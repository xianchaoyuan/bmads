#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

namespace Ui {
class MainWindow;
}

class ContainerWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::MainWindow *ui;

    QPointer<ContainerWidget> container_;
};

#endif // MAINWINDOW_H
