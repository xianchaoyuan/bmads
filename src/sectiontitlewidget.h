#ifndef SECTIONTITLEWIDGET_H
#define SECTIONTITLEWIDGET_H

#include <QFrame>

#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

/**
 * @brief 分区标题窗口
 * @details
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API SectionTitleWidget : public QFrame
{
    Q_OBJECT

public:
    SectionTitleWidget(SectionContent::RefPtr sc, QWidget *parent = nullptr);
    virtual ~SectionTitleWidget();

    //! 是否为活跃tab
    bool isActiveTab() const;
    void setActiveTab(bool active);

private:
    SectionContent::RefPtr sectionContent_;

    bool activeTab_{ false };
};

ADS_NAMESPACE_END

#endif // SECTIONTITLEWIDGET_H
