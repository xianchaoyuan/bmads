#ifndef INTERNAL_H
#define INTERNAL_H

#include <QSharedPointer>
#include <QSplitter>
#include <QLayout>

#include "adsglobal.h"

ADS_NAMESPACE_BEGIN

enum DropArea
{
    InvalidDropArea = 0,
    TopDropArea = 1,
    RightDropArea = 2,
    BottomDropArea = 4,
    LeftDropArea = 8,
    CenterDropArea = 16,

    AllAreas = TopDropArea | RightDropArea | BottomDropArea | LeftDropArea | CenterDropArea
};
Q_DECLARE_FLAGS(DropAreas, DropArea)

class SectionContent;
class SectionTitleWidget;
class SectionContentWidget;

/**
 * @brief content 数据
 * @details 用于内部传输
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class InternalContentData
{
public:
    InternalContentData() {}
    ~InternalContentData() {}

    QSharedPointer<SectionContent> content;
    SectionTitleWidget *titleWidget{};
    SectionContentWidget *contentWidget{};
};

QSplitter *findParentSplitter(QWidget *w);
QSplitter *findImmediateSplitter(QWidget *w);

class ContainerWidget;
class SectionWidget;

ContainerWidget *findParentContainerWidget(QWidget *w);
SectionWidget *findParentSectionWidget(QWidget *w);

void deleteEmptySplitter(ContainerWidget *cw);

ADS_NAMESPACE_END

#endif // INTERNAL_H
