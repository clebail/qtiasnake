#include <QtDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    idx = 0;
    newGame();
}

MainWindow::~MainWindow() {
}

void MainWindow::newGame() {
    game = Game(11, 11);
    gameWidget->setGame(game);
    reseauWidget->setReseau(game.getReseau());
}

void MainWindow::on_pbStep_clicked() {
    if(game.step()) {
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());

        lblMvt->setText(QString().number(game.getNbMouvement()));
    } else {
        if(idx < SIZE_GENERATION - 1) {
            generation.append(game.getResult());
            idx++;
        } else {
            generation.clear();
            idx = 0;
        }

        newGame();
    }
}
