#include "sectionwidget.h"
#include "sectiontitlewidget.h"
#include "sectioncontentwidget.h"

#include <QBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include <QStyle>
#include <QMenu>

ADS_NAMESPACE_BEGIN

SectionWidget::SectionWidget(QWidget *parent)
    : QFrame(parent)
    , uid_(getNewUid())
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    // 带有选项卡和关闭按钮的顶部区域
    topLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
    topLayout_->setContentsMargins(0, 0, 0, 0);
    topLayout_->setSpacing(0);
    l->addLayout(topLayout_);

    tabsScrollArea_ = new TabsScrollArea(this);
    topLayout_->addWidget(tabsScrollArea_, 1);

    auto *tabsContainerWidget = new QWidget();
    tabsContainerWidget->setObjectName("tabsContainerWidget");
    tabsScrollArea_->setWidget(tabsContainerWidget);

    tabsLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
    tabsLayout_->setContentsMargins(0, 0, 0, 0);
    tabsLayout_->setSpacing(0);
    tabsLayout_->addStretch(1);
    tabsContainerWidget->setLayout(tabsLayout_);

    // tabs切换按钮
    tabsMenuButton_ = new QPushButton();
    tabsMenuButton_->setObjectName("tabsMenuButton");
    tabsMenuButton_->setFlat(true);
    tabsMenuButton_->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
    tabsMenuButton_->setMaximumWidth(tabsMenuButton_->iconSize().width());
    tabsMenuButton_->setStyleSheet("QPushButton::menu-indicator{ image:none; }");
    topLayout_->addWidget(tabsMenuButton_, 0);

    // 关闭按钮
    closeButton_ = new QPushButton();
    closeButton_->setObjectName("closeButton");
    closeButton_->setFlat(true);
    closeButton_->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    closeButton_->setToolTip(tr("Close"));
    closeButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topLayout_->addWidget(closeButton_, 0);
    connect(closeButton_, &QPushButton::clicked, this, &SectionWidget::onCloseButtonClicked);

    // 中心内容区域
    contentsLayout_ = new QStackedLayout();
    contentsLayout_->setContentsMargins(0, 0, 0, 0);
    contentsLayout_->setSpacing(0);
    l->addLayout(contentsLayout_, 1);

    tabsLayoutInitCount_ = tabsLayout_->count();
}

SectionWidget::~SectionWidget()
{
}

int SectionWidget::uid() const
{
    return uid_;
}

int SectionWidget::currentIndex() const
{
    return contentsLayout_->currentIndex();
}

int SectionWidget::getNewUid()
{
    static int newUid = 0;
    return ++newUid;
}

void SectionWidget::onCloseButtonClicked()
{
    // BmTODO hide current content
}

TabsScrollArea::TabsScrollArea(SectionWidget *sectionWidget, QWidget *parent)
    : QScrollArea(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

TabsScrollArea::~TabsScrollArea()
{

}

void TabsScrollArea::wheelEvent(QWheelEvent *e)
{
    e->accept();

    const int direction = e->angleDelta().y();
    if (direction < 0)
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
    else
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
}

ADS_NAMESPACE_END
