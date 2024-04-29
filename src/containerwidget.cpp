#include "containerwidget.h"
#include "sectiontitlewidget.h"
#include "sectioncontentwidget.h"
#include "sectionwidget.h"

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

    // 如果还不存在分区，请创建一个默认分区，并始终放入其中。
    if (sectionWidgets_.count() <= 0) {
        targetSection = newSectionWidget();
        addSection(targetSection);
        area = CenterDropArea;
    }

    // 落在外部区域
    // 落在内部区域

    return ret;
}

ADS_NAMESPACE_END
