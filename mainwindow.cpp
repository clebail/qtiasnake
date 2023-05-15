#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFileDialog>
#include <QtDebug>
#include "mainwindow.h"
#include "random.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(10);
    gameWidget->showSensors(false);

    idx = bestScore = idxGeneration = bestScoreGeneration = 0;
    showGame = true;

    newGame();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame(const Reseau::Poids &poids) {
    game = Game(22, 22, poids);
    if(showGame) {
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());
    }

    lblGeneration->setText(QString().number(idxGeneration+1)+QString(" -- ")+QString().number(idx)+QString(" / ")+QString().number(SIZE_GENERATION));
}

QList<Reseau::Poids> MainWindow::fusion() const {
    QList<Reseau::Poids> result;
    int i1 = 0;
    int i2 = 1;
    int id = 0;
    int nbNeurone = game.getNbNeurone();

    Random::maxPoids += 10;
    Random::reset();

    for(int i=0;i<ELITE && i<generation.size();i++) {
        result.append(generation[i].poids);
    }

    while(result.size() < generation.size()) {
        Reseau::Poids p1 = generation[i1].poids;
        Reseau::Poids p2 = generation[i2].poids;
        Reseau::Poids pr1;
        Reseau::Poids pr2;            

        int pivot = rand() % nbNeurone;
        int idN = 0;
        bool mute = false;
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

                for(int l=0;l<lpn1.size();l++) {
                    if(idN <= pivot) {
                        lprn1.append(lpn1[l]);
                        lprn2.append(lpn2[l]);
                    } else {
                        lprn1.append(lpn2[l]);
                        lprn2.append(lpn1[l]);
                    }

                    idN++;
                }

                if(!mute && rand() % 10 >= 5) {
                    for(int l=0;l<lprn1.size();l++) {
                        lprn1[l] = Random::generePoid();
                    }
                    mute = true;
                }

                if(!mute && rand() % 10 >= 5) {
                    for(int l=0;l<lprn2.size();l++) {
                        lprn2[l] = Random::generePoid();
                    }
                    mute = true;
                }

                lprc1.append(lprn1);
                lprc2.append(lprn2);
            }

            pr1.append(lprc1);
            pr2.append(lprc2);
        }

        result.append(pr1);
        result.append(pr2);

        id++;
        i1 = (id / NB_ACCOUPLE) % generation.size();
        i2 = (i1 + 1 +id % NB_ACCOUPLE) % generation.size();
    }

    for(int i=result.size()-1;i>=SIZE_GENERATION;i--) {
        result.removeAt(i);
    }

    return result;
}

void MainWindow::on_pbStart_clicked() {
    over = false;
    timer->start();
    pbStep->setEnabled(false);
    pbStepOver->setEnabled(false);
}

void MainWindow::on_pbStop_clicked() {
    over = false;
    timer->stop();
    pbStep->setEnabled(true);
    pbStepOver->setEnabled(true);
}

void MainWindow::on_pbStep_clicked() {
    onTimer();
}

void MainWindow::on_pbStepOver_clicked() {
    over = true;
    timer->start();
    pbStep->setEnabled(false);
    pbStepOver->setEnabled(false);
}

void MainWindow::on_pbSetInterval_clicked() {
    timer->setInterval(spInterval->value());
}

void MainWindow::on_cbSensors_stateChanged(int) {
    gameWidget->showSensors(cbSensors->checkState() == Qt::CheckState::Checked);
}

void MainWindow::on_cbShowGame_stateChanged(int) {
    showGame = cbShowGame->checkState() == Qt::CheckState::Checked;
}

void MainWindow::on_pbSave_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Sauvegarde sous"), "", tr("Fichier json (*.json)"));

    if(!fileName.isEmpty()) {
        QJsonObject json;
        QJsonArray generationData;
        QFile file(fileName);

        for(int i=0;i<newGeneration.size();i++) {
            Reseau::Poids poids = newGeneration[i];
            QJsonArray generationMemberData;

            for(int j=0;j<poids.size();j++) {
                QList<QList<float> > couchePoids = poids[j];
                QJsonArray couchePoidsData;

                for(int k=0;k<couchePoids.size();k++) {
                    QList<float> neuronePoids = couchePoids[k];
                    QJsonArray neuronePoidsData;

                    for(int l=0;l<neuronePoids.size();l++) {
                        float f = neuronePoids[l];

                        neuronePoidsData.append(QJsonValue(f));
                    }

                    couchePoidsData.append(neuronePoidsData);
                }

                generationMemberData.append(couchePoidsData);
            }

            generationData.append(generationMemberData);
        }

        json["generation_idx"] = idxGeneration;
        json["generation_data"] = generationData;

        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QTextStream stream(&file);
            QJsonDocument document;
            document.setObject(json);
            QByteArray bytes = document.toJson(QJsonDocument::Indented);

            stream << bytes;

            file.close();
        }
    }
}

void MainWindow::on_pbLoad_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir"), "", tr("Fichier json (*.json)"));

    if(!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray bytes = file.readAll();
            file.close();

            QJsonParseError jsonError;
            QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
            if(jsonError.error == QJsonParseError::NoError )
            {
                if(document.isObject())
                {
                    QJsonObject jsonObj = document.object();

                    if(jsonObj.contains("generation_data") && jsonObj.contains("generation_idx")) {
                        QJsonArray generationData = jsonObj["generation_data"].toArray();
                        QJsonArray::Iterator i;

                        newGeneration.clear();
                        for(i=generationData.begin();i!=generationData.end();++i) {
                            QJsonArray generationMemberData = (*i).toArray();
                            QJsonArray::Iterator j;
                            Reseau::Poids poids;

                           for(j=generationMemberData.begin();j!=generationMemberData.end();++j) {
                               QJsonArray couchePoidsData = (*j).toArray();
                               QJsonArray::Iterator k;
                               QList<QList<float> > couchePoids;

                               for(k=couchePoidsData.begin();k!=couchePoidsData.end();++k) {
                                   QJsonArray neuronePoidsData = (*k).toArray();
                                   QJsonArray::Iterator l;
                                   QList<float> neuronePoids;

                                   for(l=neuronePoidsData.begin();l!=neuronePoidsData.end();++l) {
                                        neuronePoids.append((*l).toDouble());
                                   }

                                   couchePoids.append(neuronePoids);
                               }

                               poids.append(couchePoids);
                           }

                           newGeneration.append(poids);
                        }

                        idxGeneration = jsonObj["generation_idx"].toInt();
                        idx = 0;
                    }
                }
            }
        }
    }
}

void MainWindow::onTimer() {
    if(game.step()) {
        if(showGame || idx < ELITE) {
            gameWidget->setGame(game);
            reseauWidget->setReseau(game.getReseau());
        }

        lblMvt->setText(QString().number(game.getTotMouvement())+QString(" -- ")+QString().number(game.getNbMouvement())+QString(" / ")+QString().number(game.getMaxMouvement()));
    } else {
        if(over) {
            on_pbStop_clicked();
        }

        if(idx < SIZE_GENERATION) {
            Game::GameResult gr = game.getResult();

            if(!gr.perdu) {
                if(gr.score > bestScore) {
                    bestScore = gr.score;
                }
                if(gr.score > bestScoreGeneration) {
                    bestScoreGeneration = gr.score;
                }

                lblBestScore->setText(QString().number(bestScoreGeneration)+QString(" / ")+QString().number(bestScore));

                generation.append(gr);
            }

            poids = Reseau::Poids();
            idx++;
            if(idx < newGeneration.size()) {
                poids = newGeneration[idx];
            }           
        } else {
            qDebug() << "Survivants :" << generation.size() << bestScoreGeneration << bestScore << QString("(") + QString().number(Random::maxPoids) + QString(")");
            Game::SortGameResult sorter;
            std::sort(generation.begin(), generation.end(), sorter);
            newGeneration = fusion();

            generation.clear();
            idx = 0;
            idxGeneration++;
            poids = newGeneration[idx];
            bestScoreGeneration = 0;

            lblBestScore->setText(QString().number(bestScoreGeneration)+QString(" / ")+QString().number(bestScore));
        }       

        newGame(poids);
    }
}
