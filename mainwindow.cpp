#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(spInterval->value());

    showGame = true;

    newGame();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame() {
    game = Game(22, 22);

    if(showGame) {
        gameWidget->setGame(game);
    }

    lblMvt->setText(QString().number(game.getTotMouvement()));
}

void MainWindow::onTimer() {
    // TODO : brancher ici le solveur A*/hamiltonien.
    //   Il devra calculer le prochain cap et appeler game.setDirection(...)
    //   avant game.step(). Pour l'instant le serpent avance tout droit.

    if(game.step()) {
        if(showGame) {
            gameWidget->setGame(game);
        }
        lblMvt->setText(QString().number(game.getTotMouvement()));
    } else {
        newGame();
    }
}

void MainWindow::on_pbStart_clicked() {
    timer->start();
    pbStep->setEnabled(false);
}

void MainWindow::on_pbStop_clicked() {
    timer->stop();
    pbStep->setEnabled(true);
}

void MainWindow::on_pbStep_clicked() {
    onTimer();
}

void MainWindow::on_pbSetInterval_clicked() {
    timer->setInterval(spInterval->value());
}

void MainWindow::on_cbShowGame_stateChanged(int) {
    showGame = cbShowGame->checkState() == Qt::CheckState::Checked;
}
