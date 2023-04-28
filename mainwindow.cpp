#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    game = Game(21, 15);
    gameWidget->setGame(game);
    reseauWidget->setReseau(game.getReseau());
}

MainWindow::~MainWindow() {
}

void MainWindow::on_pbStep_clicked() {
    if(game.step()) {
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());

        lblMvt->setText(QString().number(game.getNbMouvement()));
    } else {
        game = Game(21, 15);
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());
    }
}
