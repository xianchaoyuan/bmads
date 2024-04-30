#include "icontitlewidget.h"

#include <QIcon>
#include <QString>
#include <QBoxLayout>
#include <QLabel>
#include <QStyle>

IconTitleWidget::IconTitleWidget(const QIcon &icon, const QString &title, QWidget *parent)
    : QFrame(parent)
{
    QBoxLayout *l = new QBoxLayout(QBoxLayout::LeftToRight);
    l->setContentsMargins(0, 0, 0, 0);
    setLayout(l);

    iconLabel_ = new QLabel();
    l->addWidget(iconLabel_);

    titleLabel_ = new QLabel();
    l->addWidget(titleLabel_, 1);

    QStyle *style = this->style();
    QIcon ic = style->standardPixmap(QStyle::SP_ComputerIcon);
    setIcon(ic);
    setTitle(title);
}

void IconTitleWidget::setIcon(const QIcon &icon)
{
    if (icon.isNull()) {
        iconLabel_->setPixmap(QPixmap());
        iconLabel_->setVisible(false);
    } else {
        iconLabel_->setPixmap(icon.pixmap(16, 16));
        iconLabel_->setVisible(true);
    }
}

void IconTitleWidget::setTitle(const QString &title)
{
    if (title.isEmpty()) {
        titleLabel_->setText(QString());
        titleLabel_->setVisible(false);
    } else {
        titleLabel_->setText(title);
        titleLabel_->setVisible(true);
    }
}
