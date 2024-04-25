#include "sectioncontentwidget.h"

#include <QBoxLayout>

ADS_NAMESPACE_BEGIN

SectionContentWidget::SectionContentWidget(SectionContent::RefPtr sc, QWidget *parent)
    : QFrame(parent)
    , sectionContent_(sc)
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(sectionContent_->contentWidget());
    setLayout(l);
}

SectionContentWidget::~SectionContentWidget()
{
    layout()->removeWidget(sectionContent_->contentWidget());
}

ADS_NAMESPACE_END
