#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit_app()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::exit_app(){
    printf("EXITAPP\n");
    close();
}