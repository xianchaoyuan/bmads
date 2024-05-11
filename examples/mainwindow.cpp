#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sectioncontent.h"
#include "icontitlewidget.h"
#include "containerwidget.h"

#include <QLabel>
#include <QBoxLayout>
#include <QCalendarWidget>

static int CONTENT_COUNT = 0;

static SectionContent::RefPtr createLongTextLabelSC(ContainerWidget* container)
{
    QWidget *w = new QWidget();
    QBoxLayout *bl = new QBoxLayout(QBoxLayout::TopToBottom);
    w->setLayout(bl);

    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText(QString("Lorem Ipsum ist ein einfacher Demo-Text für die Print- und Schriftindustrie. Lorem Ipsum ist in der Industrie bereits der Standard Demo-Text seit 1500, als ein unbekannter Schriftsteller eine Hand voll Wörter nahm und diese durcheinander warf um ein Musterbuch zu erstellen. Es hat nicht nur 5 Jahrhunderte überlebt, sondern auch in Spruch in die elektronische Schriftbearbeitung geschafft (bemerke, nahezu unverändert). Bekannt wurde es 1960, mit dem erscheinen von Letrase, welches Passagen von Lorem Ipsum enhielt, so wie Desktop Software wie Aldus PageMaker - ebenfalls mit Lorem Ipsum."));
    bl->addWidget(l);

    const int index = ++CONTENT_COUNT;
    SectionContent::RefPtr sc = SectionContent::newSectionContent(QString("uname-%1").arg(index), new IconTitleWidget(QIcon(), QString("Label %1").arg(index)), w, container);
    sc->setTitle("label " + QString::number(index));
    return sc;
}

static SectionContent::RefPtr createCalendarSC(ContainerWidget *container)
{
    QCalendarWidget* w = new QCalendarWidget();

    const int index = ++CONTENT_COUNT;
    SectionContent::RefPtr sc = SectionContent::newSectionContent(QString("uname-%1").arg(index), new IconTitleWidget(QIcon(), QString("Label %1").arg(index)), w, container);
    sc->setTitle("calendar " + QString::number(index));
    return sc;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    container_ = new ContainerWidget();
    setCentralWidget(container_);

    SectionWidget *sw = nullptr;
    sw = container_->addSectionContent(createLongTextLabelSC(container_), sw, LeftDropArea);
    sw = container_->addSectionContent(createCalendarSC(container_), sw, RightDropArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);

    QMenu *menu = container_->createContextMenu();
    menu->exec(QCursor::pos());
    delete menu;
}
