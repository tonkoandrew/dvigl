#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "workerthread.h"


namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    WorkerThread *workerThread;
private:
    Ui::MainWindow* ui;


private slots:
    void exit_app();
};

#endif // MAINWINDOW_H
