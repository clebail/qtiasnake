#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::on_pbStart_clicked() {
    gameWidget->setGame(Game(12, 12));
}

