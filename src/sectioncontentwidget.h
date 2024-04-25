#ifndef SECTIONCONTENTWIDGET_H
#define SECTIONCONTENTWIDGET_H

#include <QFrame>

#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

/**
 * @brief 分区内容窗口
 * @details
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API SectionContentWidget : public QFrame
{
    Q_OBJECT

public:
    SectionContentWidget(SectionContent::RefPtr sc, QWidget *parent = nullptr);
    virtual ~SectionContentWidget();

private:
    SectionContent::RefPtr sectionContent_;
};

ADS_NAMESPACE_END

#endif // SECTIONCONTENTWIDGET_H
