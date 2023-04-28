#include <QtDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    idx = bestScore = 0;
    newGame();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame() {
    game = Game(11, 11);
    gameWidget->setGame(game);
    reseauWidget->setReseau(game.getReseau());

    lblGeneration->setText(QString().number(idx+1)+QString(" / ")+QString().number(SIZE_GENERATION));
}

void MainWindow::on_pbStep_clicked() {
    timer->start();
}

void MainWindow::onTimer() {
    if(game.step()) {
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());

        lblMvt->setText(QString().number(game.getNbMouvement()));
    } else {
        if(idx < SIZE_GENERATION - 1) {
            Game::GameResult gr = game.getResult();

            if(gr.score > bestScore) {
                bestScore = gr.score;
                lblBestScore->setText(QString().number(bestScore));
            }
            generation.append(gr);


            idx++;
        } else {
            Game::SortGameResult sorter;
            std::sort(generation.begin(), generation.end(), sorter);
            Reseau::Poids newPoids;

            generation.clear();
            idx = 0;
        }

        newGame();
    }
}
