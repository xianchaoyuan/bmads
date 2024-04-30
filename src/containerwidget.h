#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include <QFrame>
#include <QSplitter>
#include <QGridLayout>

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

private:
    void addSection(SectionWidget *section);
    SectionWidget *newSectionWidget();
    SectionWidget *dropContent(const InternalContentData &data, SectionWidget *targetSection, DropArea area, bool autoActive = true);

private:
    QList<SectionWidget *> sectionWidgets_;
    QList<FloatingWidget *> floatingWidgets_;

    QPointer<QGridLayout> mainLayout_;
    QPointer<QSplitter> splitter_;
    Qt::Orientation orientation_;

    QPointer<DropOverlay> dropOverlay_;
};

ADS_NAMESPACE_END

#endif // CONTAINERWIDGET_H
