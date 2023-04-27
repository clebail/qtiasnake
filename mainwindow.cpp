#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    game = Game(20, 20);
    gameWidget->setGame(game);
}

MainWindow::~MainWindow() {
}

void MainWindow::on_pbStep_clicked() {
    if(game.step()) {
        gameWidget->setGame(game);

        lblMvt->setText(QString().number(game.getNbMouvement()));
    }
}
