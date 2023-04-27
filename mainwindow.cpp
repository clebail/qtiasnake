#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    gameWidget->setGame(game);
}

MainWindow::~MainWindow() {
}

void MainWindow::on_pbXp_clicked() {
    game.step(1, 0);
    gameWidget->setGame(game);
}

void MainWindow::on_pbXm_clicked() {
    game.step(-1, 0);
    gameWidget->setGame(game);
}

void MainWindow::on_pbYp_clicked() {
    game.step(0, 1);
    gameWidget->setGame(game);
}

void MainWindow::on_pbYm_clicked() {
    game.step(0, -1);
    gameWidget->setGame(game);
}

