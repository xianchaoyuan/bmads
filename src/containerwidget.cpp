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

    dropOverlay_->setAllowedAreas(ADS_NS::AllAreas);
    dropOverlay_->showDropOverlay(this);
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

    // BmTODO 外部区域
    if (!targetSection) {
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

ADS_NAMESPACE_END
