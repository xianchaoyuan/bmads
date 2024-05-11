#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include <QFrame>
#include <QSplitter>
#include <QGridLayout>
#include <QMenu>

#include "internal.h"
#include "adsglobal.h"
#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

class SectionWidget;
class FloatingWidget;
class DropOverlay;

/**
 * @brief 总包含窗口
 * @details 用于统一管理其他窗口
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API ContainerWidget : public QFrame
{
    Q_OBJECT

    friend class SectionContent;
    friend class SectionTitleWidget;
    friend class SectionContentWidget;
    friend class SectionWidget;
    friend class FloatingWidget;

public:
    explicit ContainerWidget(QWidget *parent = nullptr);
    virtual ~ContainerWidget();

    //! 添加分区内容到分区窗口中
    SectionWidget *addSectionContent(const SectionContent::RefPtr &sc, SectionWidget *sw = nullptr, DropArea area = CenterDropArea);

    bool showSectionContent(const SectionContent::RefPtr &sc);
    bool hideSectionContent(const SectionContent::RefPtr &sc);

    //! 基于可用的SectionContents创建一个QMenu
    QMenu *createContextMenu() const;

    //! outer 放置区域
    QRect outerTopDropRect() const;
    QRect outerRightDropRect() const;
    QRect outerBottomDropRect() const;
    QRect outerLeftDropRect() const;

private:
    void addSection(SectionWidget *section);
    SectionWidget *sectionAt(const QPoint &pos) const;
    SectionWidget *newSectionWidget();
    SectionWidget *dropContent(const InternalContentData &data, SectionWidget *targetSection, DropArea area, bool autoActive = true);
    SectionWidget *dropContentOuterHelper(const InternalContentData &data, Qt::Orientation orientation, QLayout *l, bool append);

private slots:
    void onActionToggleSectionContentVisibility(bool visible);

private:
    QList<SectionWidget *> sectionWidgets_;
    QList<FloatingWidget *> floatingWidgets_;
    QHash<int, HiddenSectionItem> hiddenSectionContents_;

    // 查找助手
    QHash<int, SectionWidget *> swLookupMapById_;
    QHash<int, SectionContent::WeakPtr> scLookupMapById_;
    QHash<QString, SectionContent::WeakPtr> scLookupMapByName_;

    QPointer<QGridLayout> mainLayout_;
    QPointer<QSplitter> splitter_;
    Qt::Orientation orientation_;

    QPointer<DropOverlay> dropOverlay_;
};

ADS_NAMESPACE_END

#endif // CONTAINERWIDGET_H
