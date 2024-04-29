#include "floatingwidget.h"
#include "containerwidget.h"
#include "sectioncontentwidget.h"
#include "sectiontitlewidget.h"

#include <QPushButton>
#include <QStyle>
#include <QDebug>

ADS_NAMESPACE_BEGIN

FloatingWidget::FloatingWidget(ContainerWidget *container,
                               SectionTitleWidget *titleWidget,
                               SectionContentWidget *contentWidget,
                               SectionContent::RefPtr sc, QWidget *parent)
    : QWidget(parent, Qt::CustomizeWindowHint | Qt::Tool)
    , containerWidget_(container)
    , titleWidget_(titleWidget)
    , contentWidget_(contentWidget)
    , sectionContent_(sc)
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    titleLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
    titleLayout_->addWidget(titleWidget, 1);
    l->addLayout(titleLayout_, 0);

    QPushButton *closeButton = new QPushButton();
    closeButton->setObjectName("closeButton");
    closeButton->setFlat(true);
    closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    closeButton->setToolTip(tr("Close"));
    closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    titleLayout_->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &FloatingWidget::onCloseButtonClicked);

    l->addWidget(contentWidget_, 1);
    contentWidget_->show();

    containerWidget_->floatingWidgets_.append(this);
}

FloatingWidget::~FloatingWidget()
{
    containerWidget_->floatingWidgets_.removeAll(this);
}

void FloatingWidget::onCloseButtonClicked()
{
    // BmTODO hide section
}

ADS_NAMESPACE_END
