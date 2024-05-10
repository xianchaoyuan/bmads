#ifndef SECTIONTITLEWIDGET_H
#define SECTIONTITLEWIDGET_H

#include <QFrame>

#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

class FloatingWidget;

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
    Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)

public:
    SectionTitleWidget(SectionContent::RefPtr sc, QWidget *parent = nullptr);
    virtual ~SectionTitleWidget();

    //! 是否为活跃tab
    bool isActiveTab() const;
    void setActiveTab(bool active);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

signals:
    void clicked();
    void activeTabChanged();

private:
    SectionContent::RefPtr sectionContent_;

    QPointer<FloatingWidget> floatingWidget_;
    QPoint dragStartPos_;

    bool tabMoving_{ false };
    bool activeTab_{ false };
};

ADS_NAMESPACE_END

#endif // SECTIONTITLEWIDGET_H
