#include "containerwidget.h"
#include "sectiontitlewidget.h"
#include "sectioncontentwidget.h"
#include "sectionwidget.h"
#include "floatingwidget.h"
#include "dropoverlay.h"

#include <QSplitter>
#include <QVariant>
#include <QAction>
#include <QDebug>

ADS_NAMESPACE_BEGIN

static QSplitter *newSplitter(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr)
{
    QSplitter *s = new QSplitter(orientation, parent);
    s->setProperty("ads-splitter", QVariant(true));
    s->setChildrenCollapsible(false);
    s->setOpaqueResize(false);
    return s;
}

ContainerWidget::ContainerWidget(QWidget *parent)
    : orientation_(Qt::Horizontal)
    , dropOverlay_(new DropOverlay(this))
{
    mainLayout_ = new QGridLayout();
    mainLayout_->setContentsMargins(9, 9, 9, 9);
    mainLayout_->setSpacing(0);
    setLayout(mainLayout_);
}

ContainerWidget::~ContainerWidget()
{
    while (!sectionWidgets_.isEmpty()) {
        auto *sw = sectionWidgets_.takeLast();
        sw->deleteLater();
    }
    while (!floatingWidgets_.isEmpty()) {
        auto *fw = floatingWidgets_.takeLast();
        fw->deleteLater();
    }
}

SectionWidget *ContainerWidget::addSectionContent(const SectionContent::RefPtr &sc, SectionWidget *sw, DropArea area)
{
    InternalContentData data;
    data.content = sc;
    data.titleWidget = new SectionTitleWidget(sc, nullptr);
    data.contentWidget = new SectionContentWidget(sc, nullptr);

    return dropContent(data, sw, area, false);
}

bool ContainerWidget::showSectionContent(const SectionContent::RefPtr &sc)
{
    // 在floatings中搜索
    for (int i = 0; i < floatingWidgets_.count(); ++i) {
        FloatingWidget *fw = floatingWidgets_.at(i);
        const bool found = fw->sectionContent()->uid() == sc->uid();
        if (!found)
            continue;
        fw->setVisible(true);
        fw->titleWidget_->setVisible(true);
        fw->contentWidget_->setVisible(true);
        return true;
    }

    // 在隐藏部分中搜索
    // 试着在最后一个位置显示它们，否则只需将其附加到第一个部分
    if (hiddenSectionContents_.contains(sc->uid())) {
        const HiddenSectionItem hsi = hiddenSectionContents_.take(sc->uid());
        hsi.data.titleWidget->setVisible(true);
        hsi.data.contentWidget->setVisible(true);
        SectionWidget *sw = nullptr;
        if (hsi.preferredSectionId > 0 && (sw = SWLookupMapById(this).value(hsi.preferredSectionId)) != nullptr) {
            sw->addContent(hsi.data, true);
            return true;
        } else if (sectionWidgets_.size() > 0 && (sw = sectionWidgets_.first()) != nullptr) {
            sw->addContent(hsi.data, true);
            return true;
        } else {
            sw = newSectionWidget();
            addSection(sw);
            sw->addContent(hsi.data, true);
            return true;
        }
    }

    qWarning("Unable to show SectionContent, don't know where 8-/ (already visible?)");
    return false;
}

bool ContainerWidget::hideSectionContent(const SectionContent::RefPtr &sc)
{
    // 在floatings中搜索
    for (int i = 0; i < floatingWidgets_.count(); ++i) {
        const bool found = floatingWidgets_.at(i)->sectionContent()->uid() == sc->uid();
        if (!found)
            continue;
        floatingWidgets_.at(i)->setVisible(false);
        return true;
    }

    // 在sections中查找
    // 需要将SC从SW中完全移除，并将其固定在单独的列表，只要再次为SC调用 showSectionContent 即可。
    for (int i = 0; i < sectionWidgets_.count(); ++i) {
        SectionWidget *sw = sectionWidgets_.at(i);
        const bool found = sw->indexOfContent(sc) >= 0;
        if (!found)
            continue;

        HiddenSectionItem hsi;
        hsi.preferredSectionId = sw->uid();
        hsi.preferredSectionIndex = sw->indexOfContent(sc);
        if (!sw->takeContent(sc->uid(), hsi.data))
            return false;

        hsi.data.titleWidget->setVisible(false);
        hsi.data.contentWidget->setVisible(false);
        hiddenSectionContents_.insert(sc->uid(), hsi);

        // 如果SW没有任何其他SC，我们需要将其删除。
        if (sw->sectionContents().isEmpty()) {
            delete sw;
            sw = nullptr;
            deleteEmptySplitter(this);
        }
        return true;
    }

    if (hiddenSectionContents_.contains(sc->uid()))
        return true;

    return false;
}

QMenu *ContainerWidget::createContextMenu() const
{
    QMap<QString, QAction *> actions;

    // Visible contents of sections
    for (int i = 0; i < sectionWidgets_.size(); ++i) {
        const SectionWidget *sw = sectionWidgets_.at(i);
        const QList<SectionContent::RefPtr> &contents = sw->sectionContents();
        foreach (const SectionContent::RefPtr &sc, contents) {
            QAction *a = new QAction(QIcon(), sc->title(), nullptr);
            a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
            a->setProperty("uid", sc->uid());
            a->setProperty("type", "section");
            a->setCheckable(true);
            a->setChecked(true);
            QObject::connect(a, &QAction::toggled, this, &ContainerWidget::onActionToggleSectionContentVisibility);

            actions.insert(a->text(), a);
        }
    }

    // Hidden contents of sections
    QHashIterator<int, HiddenSectionItem> hiddenIter(hiddenSectionContents_);
    while (hiddenIter.hasNext()) {
        hiddenIter.next();
        const SectionContent::RefPtr sc = hiddenIter.value().data.content;

        QAction *a = new QAction(QIcon(), sc->title(), nullptr);
        a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
        a->setProperty("uid", sc->uid());
        a->setProperty("type", "section");
        a->setCheckable(true);
        a->setChecked(false);
        QObject::connect(a, &QAction::toggled, this, &ContainerWidget::onActionToggleSectionContentVisibility);

        actions.insert(a->text(), a);
    }

    // Floating contents
    for (int i = 0; i < floatingWidgets_.size(); ++i) {
        const FloatingWidget *fw = floatingWidgets_.at(i);
        const SectionContent::RefPtr sc = fw->sectionContent();

        QAction *a = new QAction(QIcon(), sc->title(), nullptr);
        a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
        a->setProperty("uid", sc->uid());
        a->setProperty("type", "floating");
        a->setCheckable(true);
        a->setChecked(fw->isVisible());
        QObject::connect(a, &QAction::toggled, this, &ContainerWidget::onActionToggleSectionContentVisibility);

        actions.insert(a->text(), a);
    }

    QMenu *m = new QMenu(nullptr);
    m->addActions(actions.values());
    return m;
}

QRect ContainerWidget::outerTopDropRect() const
{
    QRect r = rect();
    int h = r.height() / 100 * 5;
    return QRect(r.left(), r.top(), r.width(), h);
}

QRect ContainerWidget::outerRightDropRect() const
{
    QRect r = rect();
    int w = r.width() / 100 * 5;
    return QRect(r.right() - w, r.top(), w, r.height());
}

QRect ContainerWidget::outerBottomDropRect() const
{
    QRect r = rect();
    int h = r.height() / 100 * 5;
    return QRect(r.left(), r.bottom() - h, r.width(), h);
}

QRect ContainerWidget::outerLeftDropRect() const
{
    QRect r = rect();
    int w = r.width() / 100 * 5;
    return QRect(r.left(), r.top(), w, r.height());
}

void ContainerWidget::addSection(SectionWidget *section)
{
    // 创建默认分割器
    if (!splitter_) {
        splitter_ = newSplitter(orientation_);
        mainLayout_->addWidget(splitter_, 0, 0);
    }
    if (splitter_->indexOf(section) != -1) {
        qWarning() << Q_FUNC_INFO << QString("Section has already been added");
        return;
    }
    splitter_->addWidget(section);
}

SectionWidget *ContainerWidget::sectionAt(const QPoint &pos) const
{
    const QPoint gpos = mapToGlobal(pos);
    for (int i = 0; i < sectionWidgets_.size(); ++i) {
        SectionWidget *sw = sectionWidgets_[i];
        if (sw->rect().contains(sw->mapFromGlobal(gpos))) {
            return sw;
        }
    }
    return nullptr;
}

SectionWidget *ContainerWidget::newSectionWidget()
{
    SectionWidget *sw = new SectionWidget(this);
    sectionWidgets_.append(sw);
    return sw;
}

SectionWidget *ContainerWidget::dropContent(const InternalContentData &data, SectionWidget *targetSection, DropArea area, bool autoActive)
{
    SectionWidget *ret = nullptr;

    // 如果还不存在分区，请创建一个默认分区。
    if (sectionWidgets_.count() <= 0) {
        targetSection = newSectionWidget();
        addSection(targetSection);
        area = CenterDropArea;
    }

    // 外部区域
    if (!targetSection) {
        switch (area)
        {
        case TopDropArea:
            ret = dropContentOuterHelper(data, Qt::Vertical, mainLayout_, false);
            break;
        case RightDropArea:
            ret = dropContentOuterHelper(data, Qt::Horizontal, mainLayout_, true);
            break;
        case CenterDropArea:
        case BottomDropArea:
            ret = dropContentOuterHelper(data, Qt::Vertical, mainLayout_, true);
            break;
        case LeftDropArea:
            ret = dropContentOuterHelper(data, Qt::Horizontal, mainLayout_, false);
            break;
        default:
            return nullptr;
        }
        return ret;
    }

    QSplitter *targetSectionSplitter = findParentSplitter(targetSection);

    // 内部区域
    switch (area) {
    case TopDropArea: {
        SectionWidget *sw = newSectionWidget();
        sw->addContent(data, true);
        if (targetSectionSplitter->orientation() == Qt::Vertical) {
            const int index = targetSectionSplitter->indexOf(targetSection);
            targetSectionSplitter->insertWidget(index, sw);
        } else {
            const int index = targetSectionSplitter->indexOf(targetSection);
            QSplitter *s = newSplitter(Qt::Vertical);
            s->addWidget(sw);
            s->addWidget(targetSection);
            targetSectionSplitter->insertWidget(index, s);
        }
        ret = sw;
        break;
    }
    case RightDropArea: {
        SectionWidget *sw = newSectionWidget();
        sw->addContent(data, true);
        if (targetSectionSplitter->orientation() == Qt::Horizontal) {
            const int index = targetSectionSplitter->indexOf(targetSection);
            targetSectionSplitter->insertWidget(index + 1, sw);
        } else {
            const int index = targetSectionSplitter->indexOf(targetSection);
            QSplitter *s = newSplitter(Qt::Horizontal);
            s->addWidget(targetSection);
            s->addWidget(sw);
            targetSectionSplitter->insertWidget(index, s);
        }
        ret = sw;
        break;
    }
    case BottomDropArea: {
        SectionWidget *sw = newSectionWidget();
        sw->addContent(data, true);
        if (targetSectionSplitter->orientation() == Qt::Vertical) {
            int index = targetSectionSplitter->indexOf(targetSection);
            targetSectionSplitter->insertWidget(index + 1, sw);
        } else {
            int index = targetSectionSplitter->indexOf(targetSection);
            QSplitter *s = newSplitter(Qt::Vertical);
            s->addWidget(targetSection);
            s->addWidget(sw);
            targetSectionSplitter->insertWidget(index, s);
        }
        ret = sw;
        break;
    }
    case LeftDropArea: {
        SectionWidget *sw = newSectionWidget();
        sw->addContent(data, true);
        if (targetSectionSplitter->orientation() == Qt::Horizontal) {
            int index = targetSectionSplitter->indexOf(targetSection);
            targetSectionSplitter->insertWidget(index, sw);
        } else {
            QSplitter *s = newSplitter(Qt::Horizontal);
            s->addWidget(sw);
            int index = targetSectionSplitter->indexOf(targetSection);
            targetSectionSplitter->insertWidget(index, s);
            s->addWidget(targetSection);
        }
        ret = sw;
        break;
    }
    case CenterDropArea: {
        targetSection->addContent(data);
        ret = targetSection;
        break;
    }
    default:
        break;
    }

    return ret;
}

SectionWidget *ContainerWidget::dropContentOuterHelper(const InternalContentData &data, Qt::Orientation orientation, QLayout *l, bool append)
{
    SectionWidget *sw = newSectionWidget();
    sw->addContent(data, true);

    QSplitter *oldsp = findImmediateSplitter(this);
    if (!oldsp) {
        QSplitter *sp = newSplitter(orientation);
        if (l->count() > 0) {
            ADS_PRINT("Still items in layout. This should never happen.");
            QLayoutItem *li = l->takeAt(0);
            delete li;
        }
        l->addWidget(sp);
        sp->addWidget(sw);
    } else if (oldsp->orientation() == orientation || oldsp->count() == 1) {
        oldsp->setOrientation(orientation);
        if (append)
            oldsp->addWidget(sw);
        else
            oldsp->insertWidget(0, sw);
    } else {
        QSplitter *sp = newSplitter(orientation);
        if (append) {
            QLayoutItem *li = l->replaceWidget(oldsp, sp);
            sp->addWidget(oldsp);
            sp->addWidget(sw);
            delete li;
        } else {
            sp->addWidget(sw);
            QLayoutItem *li = l->replaceWidget(oldsp, sp);
            sp->addWidget(oldsp);
            delete li;
        }
    }
    return sw;
}

void ContainerWidget::onActionToggleSectionContentVisibility(bool visible)
{
    QAction *act = qobject_cast<QAction *>(sender());
    if (!act)
        return;
    const int uid = act->property("uid").toInt();
    const SectionContent::RefPtr sc = SCLookupMapById(this).value(uid).toStrongRef();
    if (sc.isNull()) {
        qCritical() << "Can not find content by ID" << uid;
        return;
    }

    if (visible)
        showSectionContent(sc);
    else
        hideSectionContent(sc);
}

ADS_NAMESPACE_END
