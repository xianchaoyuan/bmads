#include "sectioncontent.h"
#include "containerwidget.h"
#include "internal.h"

ADS_NAMESPACE_BEGIN

SectionContent::SectionContent()
    : uid_(getNewUid())
{

}

SectionContent::~SectionContent()
{
    if (containerWidget_) {
        SCLookupMapById(containerWidget_).remove(uid_);
        SCLookupMapByName(containerWidget_).remove(uniqueName_);
    }

    titleWidget_->deleteLater();
    contentWidget_->deleteLater();
}

QWidget *SectionContent::titleWidget() const
{
    return titleWidget_;
}

QWidget *SectionContent::contentWidget() const
{
    return contentWidget_;
}

int SectionContent::getNewUid()
{
    static int newUid = 0;
    return ++newUid;
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString &uniqueName, QWidget *titleWidget, QWidget *contentWidget, ContainerWidget *containerWidget)
{
    if (uniqueName.isEmpty()) {
        qFatal("Can not create SectionContent with empty uniqueName");
        return RefPtr{};
    } else if (!titleWidget || !contentWidget || !containerWidget) {
        qFatal("Can not create SectionContent with NULL values");
        return RefPtr{};
    } else if (SCLookupMapByName(containerWidget).contains(uniqueName)) {  // 已经存在
        qFatal("Can not create SectionContent with already used uniqueName");
        return RefPtr();
    }

    RefPtr sc(new SectionContent());
    sc->uniqueName_ = uniqueName;
    sc->titleWidget_ = titleWidget;
    sc->contentWidget_ = contentWidget;
    sc->containerWidget_ = containerWidget;

    SCLookupMapById(containerWidget).insert(sc->uid(), sc);
    SCLookupMapByName(containerWidget).insert(sc->uniqueName(), sc);

    return sc;
}

ADS_NAMESPACE_END
