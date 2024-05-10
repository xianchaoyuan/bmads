#include <QApplication>
#include <QFile>

#include "mainwindow.h"

static void initStyleSheet(QApplication& a)
{
    QFile f(":ads/resource/style/default-windows.css");
    if (f.open(QFile::ReadOnly)) {
        const QByteArray ba = f.readAll();
        f.close();
        a.setStyleSheet(QString(ba));
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initStyleSheet(a);
    MainWindow w;
    w.show();
	return a.exec();
}
