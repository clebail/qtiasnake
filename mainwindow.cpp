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

void MainWindow::newGame(const Reseau::Poids &poids) {
    game = Game(11, 11, poids);
    gameWidget->setGame(game);
    reseauWidget->setReseau(game.getReseau());

    lblGeneration->setText(QString().number(idx+1)+QString(" / ")+QString().number(SIZE_GENERATION));
}

QList<Reseau::Poids>  MainWindow::fusion() const {
    QList<Reseau::Poids> result;

    for(int i=0;i<SIZE_GENERATION/2;i++) {
        for(int j=i+1;j<SIZE_GENERATION/2;j++) {
            result.append(fusion(generation[i].poids, generation[j].poids));
        }
    }

     qDebug() << generation[0].poids.size() << result.size();

    return result;
}

Reseau::Poids MainWindow::fusion(const Reseau::Poids &p1, const Reseau::Poids &p2) const {
    Reseau::Poids poids;

    for(int i=0;i<p1.size();i++) {
        poids.append(fusion(p1[i], p2[i]));
    }

    return poids;
}

QList<QList<float> > MainWindow::fusion(const QList<QList<float> > &l1, const QList<QList<float> > &l2) const {
    QList<QList<float> > result;

    for(int i=0;i<l1.size();i++) {
        result.append(fusion(l1[i], l2[i]));
    }

    return result;
}

QList<float> MainWindow::fusion(const QList<float> &l1, const QList<float> &l2) const {
    QList<float> result;

    for(int i=0;i<l1.size();i++) {
        switch(rand() % 3) {
        case 0:
            result.append(l1[i]);
            break;
        case 1:
            result.append(l2[i]);
            break;
        case 2:
            result.append((l1[i] + l2[i]) / 2);
            break;
        }
    }

    return result;
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
            // todo fusion();

            generation.clear();
            idx = 0;
        }

        newGame(poids);
    }
}
