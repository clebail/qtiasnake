#include <QtDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(10);
    // gameWidget->showSensors(false);

    idx = bestScore = idxGeneration = 0;
    newGame();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame(const Reseau::Poids &poids) {
    game = Game(11, 11, poids);
    gameWidget->setGame(game);
    reseauWidget->setReseau(game.getReseau());

    lblGeneration->setText(QString().number(idxGeneration+1)+QString(" / ")+QString().number(idx+1)+QString(" / ")+QString().number(SIZE_GENERATION));
}

QList<Reseau::Poids>  MainWindow::fusion() const {
    QList<Reseau::Poids> result;
    int i1 = 0;
    int i2 = 1;
    int id = 0;

    while(result.size() < generation.size()) {
        Reseau::Poids p1 = generation[i1].poids;
        Reseau::Poids p2 = generation[i2].poids;
        Reseau::Poids pr1;
        Reseau::Poids pr2;

        for(int j=0;j<p1.size();j++) {
            QList<QList<float> > lpc1 = p1[j];
            QList<QList<float> > lpc2 = p2[j];
            QList<QList<float> > lprc1;
            QList<QList<float> > lprc2;

            for(int k=0;k<lpc1.size();k++) {
                QList<float> lpn1 = lpc1[k];
                QList<float> lpn2 = lpc2[k];
                QList<float> lprn1;
                QList<float> lprn2;
                int pivot = rand() % lpn1.size();

                for(int l=0;l<lpn1.size();l++) {
                    if(l <= pivot) {
                        lprn1.append(lpn1[l]);
                        lprn2.append(lpn2[l]);
                    } else {
                        lprn1.append(lpn2[l]);
                        lprn2.append(lpn1[l]);
                    }
                }

                lprn1[rand() % lpn1.size()] = Neurone::generePoid();
                lprn2[rand() % lpn2.size()] = Neurone::generePoid();

                lprc1.append(lprn1);
                lprc2.append(lprn2);
            }

            pr1.append(lprc1);
            pr2.append(lprc2);
        }

        result.append(pr1);
        result.append(pr2);

        id++;
        i1 = (id / 2) % generation.size();
        i2 = (i2 + 1) % generation.size();
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
        if(idx < SIZE_GENERATION) {
            Game::GameResult gr = game.getResult();

            if(gr.score > bestScore) {
                bestScore = gr.score;
                lblBestScore->setText(QString().number(bestScore));
            }
            generation.append(gr);

            idx++;
            if(idx < newGeneration.size()) {
                poids = newGeneration[idx];
            }           
        } else {
            Game::SortGameResult sorter;
            std::sort(generation.begin(), generation.end(), sorter);
            newGeneration = fusion();

            generation.clear();
            idx = 0;
            idxGeneration++;
            poids = newGeneration[idx];

            timer->setInterval(10+(idxGeneration * 10));
        }       

        newGame(poids);
    }
}
