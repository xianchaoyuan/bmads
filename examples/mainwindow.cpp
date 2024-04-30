#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sectioncontent.h"
#include "icontitlewidget.h"
#include "containerwidget.h"

#include <QLabel>
#include <QBoxLayout>
#include <QCalendarWidget>

static int CONTENT_COUNT = 0;

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

    SectionWidget *sw = container_->addSectionContent(createCalendarSC(container_), sw, LeftDropArea);
}

MainWindow::~MainWindow()
{
	delete ui;
}
