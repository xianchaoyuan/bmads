#ifndef SECTIONWIDGET_H
#define SECTIONWIDGET_H

#include <QFrame>
#include <QList>
#include <QPointer>
#include <QBoxLayout>
#include <QPushButton>
#include <QStackedLayout>
#include <QScrollArea>

#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

class SectionTitleWidget;
class SectionContentWidget;

/**
 * @brief 分区窗口
 * @details 它显示一个标题选项卡，该选项卡是可单击的，并切换到标题关联的内容
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API SectionWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SectionWidget(QWidget *parent);
    virtual ~SectionWidget();

    int uid() const;
    int currentIndex() const;

    static int getNewUid();

private slots:
    void onCloseButtonClicked();

private:
    const int uid_;

    QList<SectionContent::RefPtr> sectionContents_;
    QList<SectionTitleWidget *> titleWidgets_;
    QList<SectionContentWidget *> contentWidgets_;

    QBoxLayout *topLayout_;  // 包含 tabs tabsmenu close
    QBoxLayout *tabsLayout_; // 包含 tabs
    QStackedLayout *contentsLayout_;

    QPointer<QScrollArea> tabsScrollArea_;

    QPushButton *tabsMenuButton_;
    QPushButton *closeButton_;

    int tabsLayoutInitCount_;
};

/**
 * @brief 选项卡的滚动实现
 * @details
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class TabsScrollArea : public QScrollArea
{
public:
    TabsScrollArea(SectionWidget *sectionWidget, QWidget *parent = nullptr);
    virtual ~TabsScrollArea();

protected:
    virtual void wheelEvent(QWheelEvent *);
};

#endif // SECTIONWIDGET_H

ADS_NAMESPACE_END
