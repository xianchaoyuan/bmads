#include "sectionwidget.h"
#include "sectiontitlewidget.h"
#include "sectioncontentwidget.h"
#include "containerwidget.h"

#include <QBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include <QStyle>
#include <QMenu>

ADS_NAMESPACE_BEGIN

SectionWidget::SectionWidget(ContainerWidget *parent)
    : QFrame(parent)
    , uid_(getNewUid())
    , containerWidget_(parent)
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
    if (containerWidget_) {
        containerWidget_->sectionWidgets_.removeAll(this);
    }

    // 删除空的QSpliter
    QSplitter *splitter = findParentSplitter(this);
    if (splitter && splitter->count() == 0)  {
        splitter->deleteLater();
    }
}

int SectionWidget::uid() const
{
    return uid_;
}

int SectionWidget::currentIndex() const
{
    return contentsLayout_->currentIndex();
}

void SectionWidget::addContent(const InternalContentData &data, bool autoActive)
{
    // 将标题小部件添加到选项卡栏
    titleWidgets_.append(data.titleWidget);
    tabsLayout_->insertWidget(tabsLayout_->count() - tabsLayoutInitCount_, data.titleWidget);
    data.titleWidget->setVisible(true);
    connect(data.titleWidget, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);

    // 将内容小部件添加到堆叠窗口
    contentWidgets_.append(data.contentWidget);
    contentsLayout_->addWidget(data.contentWidget);

    sectionContents_.append(data.content);
    if (sectionContents_.size() == 1) {
        setCurrentIndex(0);
    } else if (autoActive) {
        setCurrentIndex(sectionContents_.count() - 1);
    } else {
        data.titleWidget->setActiveTab(true);
    }

    updateTabsMenu();
}

bool SectionWidget::takeContent(int uid, InternalContentData &data)
{
    // 查找 sectioncontent
    SectionContent::RefPtr sc;
    int index = -1;
    for (int i = 0; i < sectionContents_.count(); i++) {
        if (sectionContents_[i]->uid() != uid) {
            continue;
        }

        index = i;
        sc = sectionContents_.takeAt(i);
        break;
    }

    if (!sc) {
        return false;
    }


    // 标题包装小部件（TAB）
    SectionTitleWidget *title = titleWidgets_.takeAt(index);
    if (title) {
        tabsLayout_->removeWidget(title);
        title->disconnect(this);
        title->setParent(containerWidget_);
    }

    // 内容包装小部件（Content）
    SectionContentWidget *content = contentWidgets_.takeAt(index);
    if (content) {
        contentsLayout_->removeWidget(content);
        content->disconnect(this);
        content->setParent(containerWidget_);
    }

    // 选择上一个选项卡作为activeTab。
    if (sectionContents_.size() > 0 && title->isActiveTab()) {
        if (index > 0)
            setCurrentIndex(index - 1);
        else
            setCurrentIndex(0);
    }
    updateTabsMenu();

    data.content = sc;
    data.titleWidget = title;
    data.contentWidget = content;
    return !data.content.isNull();
}

void SectionWidget::moveContent(int from, int to)
{
    if (from >= sectionContents_.size() || from < 0 || to >= sectionContents_.size() || to < 0 || from == to) {
        ADS_PRINT("Invalid index for tab movement") << from << to;
        tabsLayout_->update();
        return;
    }

    sectionContents_.move(from, to);
    titleWidgets_.move(from, to);
    contentWidgets_.move(from, to);

    QLayoutItem *liFrom = nullptr;
    liFrom = tabsLayout_->takeAt(from);
    tabsLayout_->insertItem(to, liFrom);
    // TODO 为什么liFrom不能释放
    // delete liFrom;

    liFrom = contentsLayout_->takeAt(from);
    contentsLayout_->insertWidget(to, liFrom->widget());
    delete liFrom;

    updateTabsMenu();
}

int SectionWidget::indexOfContent(const SectionContent::RefPtr &sc) const
{
    return sectionContents_.indexOf(sc);
}

int SectionWidget::indexOfContentByUid(int uid) const
{
    for (int i = 0; i < sectionContents_.count(); ++i) {
        if (sectionContents_[i]->uid() != uid) {
            continue;
        }
        return i;
    }
    return -1;
}

int SectionWidget::indexOfContentByTitlePos(const QPoint &pos, QWidget *exclude) const
{
    int index = -1;
    for (int i = 0; i < titleWidgets_.size(); ++i) {
        if (titleWidgets_[i]->geometry().contains(pos) && (exclude == nullptr || titleWidgets_[i] != exclude)) {
            index = i;
            break;
        }
    }
    return index;
}

QRect SectionWidget::titleAreaGeometry() const
{
    return topLayout_->geometry();
}

QRect SectionWidget::contentAreaGeometry() const
{
    return contentsLayout_->geometry();
}

int SectionWidget::getNewUid()
{
    static int newUid = 0;
    return ++newUid;
}

void SectionWidget::setCurrentIndex(int index)
{
    if (index < 0 || index > sectionContents_.count() - 1) {
        Q_ASSERT(false);
        return;
    }

    // 设置活动Tab
    for (int i = 0; i < tabsLayout_->count(); ++i) {
        QLayoutItem *item = tabsLayout_->itemAt(i);
        if (!item->widget()) {
            continue;
        }

        SectionTitleWidget *stw = dynamic_cast<SectionTitleWidget *>(item->widget());
        if (!stw) {
            continue;
        }

        if (i == index) {
            stw->setActiveTab(true);
            tabsScrollArea_->ensureWidgetVisible(stw);
        } else {
            stw->setActiveTab(false);
        }
    }

    // 设置活动Content
    contentsLayout_->setCurrentIndex(index);
}

void SectionWidget::onSectionTitleClicked()
{
    SectionTitleWidget *stw = qobject_cast<SectionTitleWidget *>(sender());
    if (stw) {
        int index = tabsLayout_->indexOf(stw);
        setCurrentIndex(index);
    }
}

void SectionWidget::onCloseButtonClicked()
{
    const int index = currentIndex();
    if (index < 0 || index > sectionContents_.size() - 1)
        return;

    SectionContent::RefPtr sc = sectionContents_.at(index);
    if (sc.isNull())
        return;

    containerWidget_->hideSectionContent(sc);
}

void SectionWidget::onTabsMenuActionTriggered(bool)
{
    QAction *act = qobject_cast<QAction *>(sender());
    if (!act) {
        return;
    }

    const int uid = act->data().toInt();
    const int index = indexOfContentByUid(uid);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

void SectionWidget::updateTabsMenu()
{
    QMenu *m = new QMenu();
    for (int i = 0; i < sectionContents_.count(); ++i) {
        const SectionContent::RefPtr &sc = sectionContents_.at(i);
        QAction *a = m->addAction(QIcon(), sc->title());
        a->setData(sc->uid());
        connect(a, &QAction::triggered, this, &SectionWidget::onTabsMenuActionTriggered);
    }
    QMenu *old = tabsMenuButton_->menu();
    tabsMenuButton_->setMenu(m);
    delete old;
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
{}

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
