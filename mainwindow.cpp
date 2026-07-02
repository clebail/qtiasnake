#include <QMessageBox>
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
    game = Game(32, 32);
    solver = Solver(&game);
    stepCount = 0;

    if(showGame) {
        gameWidget->setGame(game);
    }
    updateCounters();
}

void MainWindow::updateCounters() {
    int pasteques = game.getSnake().size() - 4;   // 4 = taille initiale du serpent
    lblPasteque->setText(QString("Pasteques : %1").arg(pasteques));
    lblStep->setText(QString("Steps : %1").arg(stepCount));
}

void MainWindow::onTimer() {
    game.setDirection(solver.next());

    bool alive = game.step();

    if(game.estGagne()) {
        // Plateau plein : on fige l'affichage et on annonce le score.
        stepCount++;
        if(showGame) {
            gameWidget->setGame(game);
        }
        updateCounters();

        timer->stop();
        pbStep->setEnabled(true);

        int pasteques = game.getSnake().size() - 4;
        QMessageBox::information(this, "Victoire",
            QString("Termine ! %1 pasteques mangees en %2 steps.")
                .arg(pasteques).arg(stepCount));
        return;
    }

    if(alive) {
        stepCount++;
        if(showGame) {
            gameWidget->setGame(game);
        }
        updateCounters();
    } else {
        // Mort : on fige l'état au lieu de redémarrer, pour inspecter le cas.
        if(showGame) {
            gameWidget->setGame(game);
        }
        updateCounters();

        timer->stop();
        pbStep->setEnabled(true);
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
