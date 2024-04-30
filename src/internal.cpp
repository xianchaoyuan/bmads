#include "internal.h"

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
