#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::on_pbStart_clicked() {
    QList<QPoint> snake;

    snake.append(QPoint(5, 5));
    snake.append(QPoint(5, 6));
    snake.append(QPoint(6, 6));
    snake.append(QPoint(6, 7));

    gameWidget->setSnake(snake);
    gameWidget->setPasteque(QPoint(7, 2));
}

