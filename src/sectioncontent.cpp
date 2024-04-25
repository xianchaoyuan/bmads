#include "sectioncontent.h"

ADS_NAMESPACE_BEGIN

SectionContent::SectionContent()
    : uid_(getNewUid())
{

}

SectionContent::~SectionContent()
{
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

SectionContent::RefPtr SectionContent::newSectionContent(const QString &uniqueName, QWidget *titleWidget, QWidget *contentWidget)
{
    if (uniqueName.isEmpty()) {
        qFatal("Can not create SectionContent with empty uniqueName");
        return RefPtr();
    } else if (!titleWidget || !contentWidget) {
        qFatal("Can not create SectionContent with NULL values");
        return RefPtr();
    }

    RefPtr sc(new SectionContent());
    sc->uniqueName_ = uniqueName;
    sc->titleWidget_ = titleWidget;
    sc->contentWidget_ = contentWidget;
    return sc;
}

ADS_NAMESPACE_END
