#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit_app()));



    workerThread = new WorkerThread();
    // connect(workerThread, &WorkerThread::resultReady, this, &handleResults);
    // connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

MainWindow::~MainWindow() {
    // workerThread.quit();
    // workerThread.wait();
    delete ui;
}

void MainWindow::exit_app()
{
    printf("EXITAPP\n");
    close();
}