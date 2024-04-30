#include "dropoverlay.h"

#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMoveEvent>

ADS_NAMESPACE_BEGIN

static QPixmap createDropIndicatorPixmap(const QPalette &pal, const QSizeF &size, DropArea dropArea)
{
    const QColor borderColor = pal.color(QPalette::Active, QPalette::Highlight);
    const QColor backgroundColor = pal.color(QPalette::Active, QPalette::Base);
    const QColor areaBackgroundColor = pal.color(QPalette::Active, QPalette::Highlight).lighter(150);

    QPixmap pm(size.width(), size.height());
    pm.fill(QColor(0, 0, 0, 0));

    QPainter p(&pm);
    QPen pen = p.pen();
    QRectF baseRect(pm.rect());

    // 填充
    p.fillRect(baseRect, backgroundColor);

    // 放置区域矩形
    if (true) {
        p.save();
        QRectF areaRect;
        QLineF areaLine;
        QLinearGradient gradient;
        switch (dropArea)
        {
        case TopDropArea:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
            gradient.setStart(areaRect.topLeft());
            gradient.setFinalStop(areaRect.bottomLeft());
            gradient.setColorAt(0.f, areaBackgroundColor);
            gradient.setColorAt(1.f, areaBackgroundColor.lighter(120));
            break;
        case RightDropArea:
            areaRect = QRectF(baseRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
            gradient.setStart(areaRect.topLeft());
            gradient.setFinalStop(areaRect.topRight());
            gradient.setColorAt(0.f, areaBackgroundColor.lighter(120));
            gradient.setColorAt(1.f, areaBackgroundColor);
            break;
        case BottomDropArea:
            areaRect = QRectF(baseRect.x(), baseRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
            gradient.setStart(areaRect.topLeft());
            gradient.setFinalStop(areaRect.bottomLeft());
            gradient.setColorAt(0.f, areaBackgroundColor.lighter(120));
            gradient.setColorAt(1.f, areaBackgroundColor);
            break;
        case LeftDropArea:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
            gradient.setStart(areaRect.topLeft());
            gradient.setFinalStop(areaRect.topRight());
            gradient.setColorAt(0.f, areaBackgroundColor);
            gradient.setColorAt(1.f, areaBackgroundColor.lighter(120));
            break;
        default:
            break;
        }
        if (areaRect.isValid()) {
            p.fillRect(areaRect, gradient);

            pen = p.pen();
            pen.setColor(borderColor);
            pen.setStyle(Qt::DashLine);
            p.setPen(pen);
            p.drawLine(areaLine);
        }
        p.restore();
    }

    // 边框
    if (true) {
        p.save();
        pen = p.pen();
        pen.setColor(borderColor);
        pen.setWidth(1);

        p.setPen(pen);
        p.drawRect(baseRect.adjusted(0, 0, -pen.width(), -pen.width()));
        p.restore();
    }
    return pm;
}

static QWidget *createDropIndicatorWidget(DropArea dropArea)
{
    QLabel *lab = new QLabel();
    lab->setObjectName("DropAreaLabel");

    const qreal metric = static_cast<qreal>(lab->fontMetrics().height()) * 2.f;
    const QSizeF size(metric, metric);

    lab->setPixmap(createDropIndicatorPixmap(lab->palette(), size, dropArea));
    return lab;
}

DropOverlay::DropOverlay(QWidget *parent)
    : cross_(new DropOverlayCross(this))
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowOpacity(0.2);
    setWindowTitle("DropOverlay");

    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    // 与默认放置区域小部件交叉
    QHash<DropArea, QWidget *> areaWidgets;
    areaWidgets.insert(ADS_NS::TopDropArea, createDropIndicatorWidget(TopDropArea));
    areaWidgets.insert(ADS_NS::RightDropArea, createDropIndicatorWidget(RightDropArea));
    areaWidgets.insert(ADS_NS::BottomDropArea, createDropIndicatorWidget(BottomDropArea));
    areaWidgets.insert(ADS_NS::LeftDropArea, createDropIndicatorWidget(LeftDropArea));
    areaWidgets.insert(ADS_NS::CenterDropArea, createDropIndicatorWidget(CenterDropArea));
    cross_->setAreaWidgets(areaWidgets);
    cross_->setVisible(false);

    setVisible(false);
}

DropOverlay::~DropOverlay()
{}

void DropOverlay::setAllowedAreas(DropAreas areas)
{
    if (areas == allowedAreas_)
        return;
    allowedAreas_ = areas;
    cross_->reset();
}

DropAreas DropOverlay::allowedAreas() const
{
    return allowedAreas_;
}

DropArea DropOverlay::showDropOverlay(QWidget *target)
{
    if (targetWidget_ == target) {
        // 提示：我们可以在这里更新覆盖的几何图形。
        DropArea dropArea = cursorLocation();
        if (dropArea != lastLocation_) {
            repaint();
            lastLocation_ = dropArea;
        }
        return dropArea;
    }

    hideDropOverlay();
    fullAreaDrop_ = false;
    targetWidget_ = target;
    targetWidgetRect_ = QRect();
    lastLocation_ = InvalidDropArea;

    // 将其移动到目标区域上方
    resize(target->size());
    move(target->mapToGlobal(target->rect().topLeft()));

    show();

    return cursorLocation();
}

void DropOverlay::showDropOverlay(QWidget *target, const QRect &targetAreaRect)
{
    if (targetWidget_ == target && targetWidgetRect_ == targetAreaRect) {
        return;
    }

    hideDropOverlay();
    fullAreaDrop_ = true;
    targetWidget_ = target;
    targetWidgetRect_ = targetAreaRect;
    lastLocation_ = InvalidDropArea;

    // 将其移动到目标区域上方
    resize(targetAreaRect.size());
    move(target->mapToGlobal(QPoint(targetAreaRect.x(), targetAreaRect.y())));

    show();
}

void DropOverlay::hideDropOverlay()
{
    hide();
    fullAreaDrop_ = false;
    targetWidget_.clear();
    targetWidgetRect_ = QRect();
    lastLocation_ = InvalidDropArea;
}

DropArea DropOverlay::cursorLocation() const
{
    return cross_->cursorLocation();
}

void DropOverlay::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    const QColor areaColor = palette().color(QPalette::Disabled, QPalette::Highlight); //QColor(0, 100, 255)

    // 始终在整个矩形上绘制下拉矩形
    if (fullAreaDrop_) {
        QRect r = rect();
        p.fillRect(r, QBrush(areaColor, Qt::Dense4Pattern));
        p.setBrush(QBrush(areaColor));
        p.drawRect(r);
        return;
    }

    // 根据位置绘制矩形
    QRect r = rect();
    const DropArea da = cursorLocation();
    switch (da)
    {
    case ADS_NS::TopDropArea:
        r.setHeight(r.height() / 2);
        break;
    case ADS_NS::RightDropArea:
        r.setX(r.width() / 2);
        break;
    case ADS_NS::BottomDropArea:
        r.setY(r.height() / 2);
        break;
    case ADS_NS::LeftDropArea:
        r.setWidth(r.width() / 2);
        break;
    case ADS_NS::CenterDropArea:
        r = rect();
        break;
    default:
        r = QRect();
    }
    if (!r.isNull()) {
        p.fillRect(r, QBrush(areaColor, Qt::Dense4Pattern));
        p.setBrush(QBrush(areaColor));
        p.drawRect(r);
    }
}

void DropOverlay::showEvent(QShowEvent *e)
{
    cross_->show();
}

void DropOverlay::hideEvent(QHideEvent *e)
{
    cross_->hide();
}

void DropOverlay::resizeEvent(QResizeEvent *e)
{
    cross_->resize(e->size());
}

void DropOverlay::moveEvent(QMoveEvent *e)
{
    cross_->move(e->pos());
}

static QPair<QPoint, int> gridPosForArea(const DropArea area)
{
    switch (area)
    {
    case ADS_NS::TopDropArea:
        return qMakePair(QPoint(0, 1), (int) Qt::AlignHCenter | Qt::AlignBottom);
    case ADS_NS::RightDropArea:
        return qMakePair(QPoint(1, 2), (int) Qt::AlignLeft | Qt::AlignVCenter);
    case ADS_NS::BottomDropArea:
        return qMakePair(QPoint(2, 1), (int) Qt::AlignHCenter | Qt::AlignTop);
    case ADS_NS::LeftDropArea:
        return qMakePair(QPoint(1, 0), (int) Qt::AlignRight | Qt::AlignVCenter);
    case ADS_NS::CenterDropArea:
        return qMakePair(QPoint(1, 1), (int) Qt::AlignCenter);
    default:
        return QPair<QPoint, int>();
    }
}

DropOverlayCross::DropOverlayCross(DropOverlay *overlay)
    : QWidget(overlay->parentWidget())
    , overlay_(overlay)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    gridLayout_ = new QGridLayout();
    gridLayout_->setContentsMargins(0, 0, 0, 0);
    gridLayout_->setSpacing(6);

    QBoxLayout *bl1 = new QBoxLayout(QBoxLayout::TopToBottom);
    bl1->setContentsMargins(0, 0, 0, 0);
    bl1->setSpacing(0);
    setLayout(bl1);

    QBoxLayout *bl2 = new QBoxLayout(QBoxLayout::LeftToRight);
    bl2->setContentsMargins(0, 0, 0, 0);
    bl2->setSpacing(0);

    bl1->addStretch(1);
    bl1->addLayout(bl2);
    bl2->addStretch(1);
    bl2->addLayout(gridLayout_, 0);
    bl2->addStretch(1);
    bl1->addStretch(1);
}

DropOverlayCross::~DropOverlayCross()
{}

void DropOverlayCross::setAreaWidgets(const QHash<DropArea, QWidget *> &widgets)
{
    // 删除旧的小部件
    QMutableHashIterator<DropArea, QWidget *> i(widgets_);
    while (i.hasNext()) {
        i.next();
        QWidget *widget = i.value();
        gridLayout_->removeWidget(widget);
        delete widget;
        i.remove();
    }

    // 将新的小部件插入
    widgets_ = widgets;
    QHashIterator<DropArea, QWidget *> i2(widgets_);
    while (i2.hasNext()) {
        i2.next();
        const DropArea area = i2.key();
        QWidget *widget = i2.value();
        const QPair<QPoint, int> opts = gridPosForArea(area);
        gridLayout_->addWidget(widget, opts.first.x(), opts.first.y(), (Qt::Alignment)opts.second);
    }
    reset();
}

DropArea DropOverlayCross::cursorLocation() const
{
    const QPoint pos = mapFromGlobal(QCursor::pos());
    QHashIterator<DropArea, QWidget *> i(widgets_);
    while (i.hasNext()) {
        i.next();
        if (overlay_->allowedAreas().testFlag(i.key())
            && i.value()
            && i.value()->isVisible()
            && i.value()->geometry().contains(pos)) {
            return i.key();
        }
    }
    return InvalidDropArea;
}

void DropOverlayCross::showEvent(QShowEvent *e)
{
    resize(overlay_->size());
    move(overlay_->pos());
}

void DropOverlayCross::reset()
{
    QList<DropArea> allAreas;
    allAreas << ADS_NS::TopDropArea
             << ADS_NS::RightDropArea
             << ADS_NS::BottomDropArea
             << ADS_NS::LeftDropArea
             << ADS_NS::CenterDropArea;
    const DropAreas allowedAreas = overlay_->allowedAreas();

    // 基于allowedAreas更新区域小部件的可见性
    for (int i = 0; i < allAreas.count(); ++i) {
        const QPair<QPoint, int> opts = gridPosForArea(allAreas.at(i));

        QLayoutItem *item = gridLayout_->itemAtPosition(opts.first.x(), opts.first.y());
        QWidget *w = nullptr;
        if (item && (w = item->widget()) != nullptr) {
            w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
        }
    }
}

ADS_NAMESPACE_END

