#include "containerwidget.h"
#include "sectiontitlewidget.h"
#include "sectioncontentwidget.h"
#include "sectionwidget.h"
#include "floatingwidget.h"
#include "dropoverlay.h"

#include <QSplitter>
#include <QVariant>
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

ADS_NAMESPACE_END
