#include "sectiontitlewidget.h"
#include "sectionwidget.h"
#include "floatingwidget.h"
#include "containerwidget.h"
#include "dropoverlay.h"

#include <QStyle>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QApplication>

ADS_NAMESPACE_BEGIN

SectionTitleWidget::SectionTitleWidget(SectionContent::RefPtr sc, QWidget *parent)
    : QFrame(parent)
    , sectionContent_(sc)
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::LeftToRight);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(sectionContent_->titleWidget());
    setLayout(l);
}

SectionTitleWidget::~SectionTitleWidget()
{
    layout()->removeWidget(sectionContent_->titleWidget());
}

bool SectionTitleWidget::isActiveTab() const
{
    return activeTab_;
}

void SectionTitleWidget::setActiveTab(bool active)
{
    if (activeTab_ == active) {
        return;
    }

    activeTab_ = active;

    // 样式刷新
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void SectionTitleWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        e->accept();
        dragStartPos_ = e->pos();
        return;
    }
    QFrame::mousePressEvent(e);
}

void SectionTitleWidget::mouseReleaseEvent(QMouseEvent *e)
{
    SectionWidget *section = nullptr;
    ContainerWidget *cw = findParentContainerWidget(this);

    // 将FloatingWidget的内容放入SectionWidget
    if (floatingWidget_) {
        SectionWidget *sw = cw->sectionAt(cw->mapFromGlobal(e->globalPos()));
        if (sw) {
            cw->dropOverlay_->setAllowedAreas(ADS_NS::AllAreas);
            DropArea loc = cw->dropOverlay_->showDropOverlay(sw);
            if (loc != InvalidDropArea) {
                InternalContentData data;
                floatingWidget_->takeContent(data);
                floatingWidget_->deleteLater();
                floatingWidget_.clear();
                cw->dropContent(data, sw, loc, true);
            }
        } else {
            // 鼠标位于外缘拖放区域
            DropArea dropArea = ADS_NS::InvalidDropArea;
            if (cw->outerTopDropRect().contains(cw->mapFromGlobal(e->globalPos())))
                dropArea = ADS_NS::TopDropArea;
            if (cw->outerRightDropRect().contains(cw->mapFromGlobal(e->globalPos())))
                dropArea = ADS_NS::RightDropArea;
            if (cw->outerBottomDropRect().contains(cw->mapFromGlobal(e->globalPos())))
                dropArea = ADS_NS::BottomDropArea;
            if (cw->outerLeftDropRect().contains(cw->mapFromGlobal(e->globalPos())))
                dropArea = ADS_NS::LeftDropArea;

            if (dropArea != ADS_NS::InvalidDropArea) {
                InternalContentData data;
                floatingWidget_->takeContent(data);
                floatingWidget_->deleteLater();
                floatingWidget_.clear();
                cw->dropContent(data, nullptr, dropArea, true);
            }
        }
    } else if (tabMoving_ && ((section = findParentSectionWidget(this)))) { // 结束移动，改变选项卡顺序
        // 查找鼠标下选项卡
        QPoint pos = e->globalPos();
        pos = section->mapFromGlobal(pos);
        const int fromIndex = section->indexOfContent(sectionContent_);
        const int toIndex = section->indexOfContentByTitlePos(pos, this);
        section->moveContent(fromIndex, toIndex);
    }

    if (!dragStartPos_.isNull())
        emit clicked();

    // 重置
    dragStartPos_ = QPoint();
    tabMoving_ = false;
    cw->dropOverlay_->hideDropOverlay();

    QFrame::mouseReleaseEvent(e);
}

void SectionTitleWidget::mouseMoveEvent(QMouseEvent *e)
{
    ContainerWidget *cw = findParentContainerWidget(this);
    SectionWidget *sw = nullptr;

    if (floatingWidget_ && (e->buttons() & Qt::LeftButton)) {
        // 移动已存在的 floatingWidget
        e->accept();

        const QPoint moveToPos = e->globalPos() - (dragStartPos_ + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
        floatingWidget_->move(moveToPos);

        // 显示下拉指示器
        if (true) {
            // 鼠标悬停在 sectionWidget上
            sw = cw->sectionAt(cw->mapFromGlobal(QCursor::pos()));
            if (sw) {
                cw->dropOverlay_->setAllowedAreas(ADS_NS::AllAreas);
                cw->dropOverlay_->showDropOverlay(sw);
                return;
            } else if (cw->outerTopDropRect().contains(cw->mapFromGlobal(QCursor::pos()))) {    // top
                cw->dropOverlay_->setAllowedAreas(ADS_NS::TopDropArea);
                cw->dropOverlay_->showDropOverlay(cw, cw->outerTopDropRect());
            } else if (cw->outerRightDropRect().contains(cw->mapFromGlobal(QCursor::pos()))) {  // right
                cw->dropOverlay_->setAllowedAreas(ADS_NS::RightDropArea);
                cw->dropOverlay_->showDropOverlay(cw, cw->outerRightDropRect());
            } else if (cw->outerBottomDropRect().contains(cw->mapFromGlobal(QCursor::pos()))) {  // bottom
                cw->dropOverlay_->setAllowedAreas(ADS_NS::BottomDropArea);
                cw->dropOverlay_->showDropOverlay(cw, cw->outerBottomDropRect());
            } else if (cw->outerLeftDropRect().contains(cw->mapFromGlobal(QCursor::pos()))) {    // left
                cw->dropOverlay_->setAllowedAreas(ADS_NS::LeftDropArea);
                cw->dropOverlay_->showDropOverlay(cw, cw->outerLeftDropRect());
            } else {
                cw->dropOverlay_->hideDropOverlay();
            }
        }
        return;
    } else if (!floatingWidget_
               && !dragStartPos_.isNull()
               && (e->buttons() & Qt::LeftButton)
               && (sw = findParentSectionWidget(this)) != nullptr
               && !sw->titleAreaGeometry().contains(sw->mapFromGlobal(e->globalPos()))) {
        // 开始拖动/浮动 sectioncontent
        e->accept();

        // 创建浮动窗口
        InternalContentData data;
        if (!sw->takeContent(sectionContent_->uid(), data)) {
            qWarning() << "THIS SHOULD NOT HAPPEN!!" << sectionContent_->uid() << sectionContent_->uniqueName();
            return;
        }

        floatingWidget_ = new FloatingWidget(cw, data.titleWidget, data.contentWidget, data.content, cw);
        floatingWidget_->resize(sw->size());

        const QPoint moveToPos = e->globalPos() - (dragStartPos_ + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
        floatingWidget_->move(moveToPos);
        floatingWidget_->show();

        // 删除旧的部分，如果它现在是空的。
        if (sw->sectionContents().isEmpty()) {
            delete sw;
            sw = nullptr;
        }
        deleteEmptySplitter(cw);
        return;
    } else if (tabMoving_) {
        // 处理当前选项卡的移动
        e->accept();
        QPoint moveToPos = mapToParent(e->pos()) - dragStartPos_;
        moveToPos.setY(0);
        move(moveToPos);
        return;
    } else if (!dragStartPos_.isNull()
               && (e->buttons() & Qt::LeftButton)
               && (e->pos() - dragStartPos_).manhattanLength() >= QApplication::startDragDistance() // 等待几个像素后再开始移动
               && sw->titleAreaGeometry().contains(sw->mapFromGlobal(e->globalPos()))) {
        // 开始在标题区域内拖动标题，以切换其在SectionWidget中的位置。
        // 1、必须左键
        // 2、移动长度大于等于基本拖放距离
        // 3、在标题区域移动
        e->accept();
        tabMoving_ = true;
        raise(); // 将当前标题小部件提升到其他选项卡之上
        return;
    }

    QFrame::mouseMoveEvent(e);
}

ADS_NAMESPACE_END

