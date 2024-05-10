#include "internal.h"
#include "containerwidget.h"
#include "sectionwidget.h"

QSplitter *findParentSplitter(QWidget *w)
{
    QSplitter *cw = nullptr;
    QWidget *next = w;
    do {
        if ((cw = dynamic_cast<QSplitter *>(next))) {
            break;
        }
        next = next->parentWidget();
    } while (next);
    return cw;
}

QSplitter *findImmediateSplitter(QWidget *w)
{
    QSplitter *sp = nullptr;
    QLayout *l = w->layout();
    if (!l || l->count() <= 0)
        return sp;

    for (int i = 0; i < l->count(); ++i) {
        QLayoutItem *li = l->itemAt(0);
        if (!li->widget())
            continue;
        if ((sp = dynamic_cast<QSplitter *>(li->widget())))
            break;
    }
    return sp;
}

ContainerWidget *findParentContainerWidget(QWidget *w)
{
    ContainerWidget *cw = nullptr;
    QWidget *next = w;
    do {
        if ((cw = dynamic_cast<ContainerWidget *>(next)))
            break;

        next = next->parentWidget();
    } while (next);
    return cw;
}

SectionWidget *findParentSectionWidget(QWidget *w)
{
    SectionWidget *cw = nullptr;
    QWidget *next = w;
    do {
        if ((cw = dynamic_cast<SectionWidget *>(next)))
            break;

        next = next->parentWidget();
    } while (next);
    return cw;
}

static bool splitterContainsSectionWidget(QSplitter *splitter)
{
    for (int i = 0; i < splitter->count(); ++i) {
        QWidget *w = splitter->widget(i);
        QSplitter *sp = qobject_cast<QSplitter *>(w);
        SectionWidget *sw = nullptr;
        if (sp && splitterContainsSectionWidget(sp))
            return true;
        else if ((sw = qobject_cast<SectionWidget *>(w)))
            return true;
    }
    return false;
}

void deleteEmptySplitter(ContainerWidget *cw)
{
    bool doAgain = false;
    do {
        doAgain = false;
        QList<QSplitter *> splitters = cw->findChildren<QSplitter *>();
        for (int i = 0; i < splitters.count(); ++i) {
            QSplitter *sp = splitters.at(i);
            if (!sp->property("ads-splitter").toBool())
                continue;
            if (sp->count() > 0 && splitterContainsSectionWidget(sp))
                continue;
            delete splitters[i];
            doAgain = true;
            break;
        }
    } while (doAgain);
}
