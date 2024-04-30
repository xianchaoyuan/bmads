#ifndef DROPOVERLAY_H
#define DROPOVERLAY_H

#include <QFrame>
#include <QGridLayout>
#include <QPointer>

#include "adsglobal.h"
#include "internal.h"

ADS_NAMESPACE_BEGIN

class DropOverlayCross;

/**
 * @brief 放置覆盖（绘制半透明矩形）
 * @details 几何图形矩形的大小取决于鼠标的位置
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API DropOverlay : public QFrame
{
    Q_OBJECT

public:
    explicit DropOverlay(QWidget *parent);
    virtual ~DropOverlay();

    void setAllowedAreas(DropAreas areas);
    DropAreas allowedAreas() const;

    DropArea showDropOverlay(QWidget *target);
    void showDropOverlay(QWidget *target, const QRect &targetAreaRect);
    void hideDropOverlay();

    //! 鼠标所在位置
    DropArea cursorLocation() const;

protected:
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void moveEvent(QMoveEvent *e) override;

private:
    DropAreas allowedAreas_;
    QPointer<DropOverlayCross> cross_;

    bool fullAreaDrop_{ false };
    DropArea lastLocation_{ InvalidDropArea };
    QPointer<QWidget> targetWidget_;
    QRect targetWidgetRect_;
};

/**
 * @brief 放置覆盖十字架
 * @details 具有5种不同放置区域可能性的十字架
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class DropOverlayCross : public QWidget
{
    Q_OBJECT

    friend class DropOverlay;

public:
    explicit DropOverlayCross(DropOverlay *overlay);
    virtual ~DropOverlayCross();

    void setAreaWidgets(const QHash<DropArea, QWidget *> &widgets);
    DropArea cursorLocation() const;

protected:
    virtual void showEvent(QShowEvent *e);

private:
    void reset();

private:
    QPointer<DropOverlay> overlay_;
    QHash<DropArea, QWidget *> widgets_;
    QGridLayout *gridLayout_;
};

ADS_NAMESPACE_END

#endif // DROPOVERLAY_H
