#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H

#include <QWidget>
#include <QPointer>
#include <QSharedPointer>

#include "adsglobal.h"

ADS_NAMESPACE_BEGIN

class ContainerWidget;

/**
 * @brief 分区内容
 * @details
 * @author bm
 * @date 2024-04-18
 * @version V1.0
 */
class ADS_EXPORT_API SectionContent
{
public:
    typedef QSharedPointer<SectionContent> RefPtr;
    typedef QWeakPointer<SectionContent> WeakPtr;

    SectionContent();
    virtual ~SectionContent();

    int uid() const { return uid_; }
    QString uniqueName() const { return uniqueName_; }

    QString title() const { return title_; }
    void setTitle(const QString &title) { title_ = title; }

    QWidget *titleWidget() const;
    QWidget *contentWidget() const;

    static int getNewUid();
    static RefPtr newSectionContent(const QString &uniqueName,
                                    QWidget *titleWidget,
                                    QWidget *contentWidget,
                                    ContainerWidget *containerWidget);

private:
    QPointer<QWidget> titleWidget_;
    QPointer<QWidget> contentWidget_;
    QPointer<ContainerWidget> containerWidget_;

    const int uid_;
    QString uniqueName_;
    QString title_;
};

ADS_NAMESPACE_END

#endif // SECTIONCONTENT_H
