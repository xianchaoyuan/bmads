#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
#include <QBoxLayout>

#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

class ContainerWidget;
class SectionTitleWidget;
class SectionContentWidget;

/**
 * @brief 分区浮动窗口
 * @details
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class FloatingWidget : public QWidget
{
    Q_OBJECT

    friend class ContainerWidget;

public:
    explicit FloatingWidget(ContainerWidget *container,
                            SectionTitleWidget *titleWidget,
                            SectionContentWidget *contentWidget,
                            SectionContent::RefPtr sc, QWidget *parent = nullptr);
    virtual ~FloatingWidget();

private slots:
    void onCloseButtonClicked();

private:
    ContainerWidget *containerWidget_;
    SectionTitleWidget *titleWidget_;
    SectionContentWidget *contentWidget_;
    SectionContent::RefPtr sectionContent_;

    QBoxLayout *titleLayout_;
};

ADS_NAMESPACE_END

#endif // FLOATINGWIDGET_H
