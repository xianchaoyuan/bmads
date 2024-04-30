#include "sectiontitlewidget.h"

#include <QStyle>
#include <QBoxLayout>

ADS_NAMESPACE_BEGIN

SectionTitleWidget::SectionTitleWidget(SectionContent::RefPtr sc, QWidget *parent)
    : QFrame(parent)
    , sectionContent_(sc)
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::LeftToRight);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(sectionContent_->titleWidget());
    setLayout(l);
}

SectionTitleWidget::~SectionTitleWidget()
{
    layout()->removeWidget(sectionContent_->titleWidget());
}

bool SectionTitleWidget::isActiveTab() const
{
    return activeTab_;
}

void SectionTitleWidget::setActiveTab(bool active)
{
    if (activeTab_ == active) {
        return;
    }

    activeTab_ = active;
}

ADS_NAMESPACE_END

