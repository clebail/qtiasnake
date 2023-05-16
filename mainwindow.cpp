#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFileDialog>
#include <QtDebug>
#include <QCryptographicHash>
#include "mainwindow.h"
#include "random.h"

#define MAX_TEST                100

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
    QString hash;
    QList<QString> newGenHashs;

    // On prend l'élite tel quel
    for(int i=0;i<ELITE && i<generation.size();i++) {
        result.append(generation[i].poids);

        newGenHashs.append(getHash(generation[i].poids));
    }

    qDebug() << "Taille tmp 1" << result.size();

    // On accouple l'elite avec double pivot à la couche
    for(id=1,i1=0,i2=1;i1<ELITE && i1<generation.size();id++) {
        if(i2 < generation.size()) {
            Reseau::Poids p1 = generation[i1].poids;
            Reseau::Poids p2 = generation[i2].poids;
            Reseau::Poids pr1;
            Reseau::Poids pr2;

            int pivot1 = rand() % nbNeurone;
            int pivot2 = pivot1 + (rand() % (nbNeurone - pivot1));

            int idN = 0;
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
                        if(idN <= pivot1 || idN > pivot2) {
                            lprn1.append(lpn1[l]);
                            lprn2.append(lpn2[l]);
                        } else {
                            lprn1.append(lpn2[l]);
                            lprn2.append(lpn1[l]);
                        }

                        idN++;
                    }

                    lprc1.append(lprn1);
                    lprc2.append(lprn2);
                }

                pr1.append(lprc1);
                pr2.append(lprc2);
            }

            QString hPr1 = getHash(pr1);
            QString hPr2 = getHash(pr2);
            if(!allreadyPass.contains(hPr1) && !newGenHashs.contains(hPr1)) {
                result.append(pr1);
            }
            newGenHashs.append(hPr1);
            if(!allreadyPass.contains(hPr2) && !newGenHashs.contains(hPr2)) {
                result.append(pr2);
            }
            newGenHashs.append(hPr2);
        }

        i1 = (id / NB_ACCOUPLE);
        i2 = (i1 + 1 + id % NB_ACCOUPLE);
    }

    qDebug() << "Taille tmp 2" << result.size();

    // On accouple l'elite avec simple pivot au neurone
    for(id=1,i1=0,i2=1;i1<ELITE && i1<generation.size();id++) {
        if(i2 < generation.size()) {
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
                        if(l >= pivot) {
                            lprn1.append(lpn1[l]);
                            lprn2.append(lpn2[l]);
                        } else {
                            lprn1.append(lpn2[l]);
                            lprn2.append(lpn1[l]);
                        }
                    }

                    lprc1.append(lprn1);
                    lprc2.append(lprn2);
                }

                pr1.append(lprc1);
                pr2.append(lprc2);
            }

            QString hPr1 = getHash(pr1);
            QString hPr2 = getHash(pr2);
            if(!allreadyPass.contains(hPr1) && !newGenHashs.contains(hPr1)) {
                result.append(pr1);
            }
            newGenHashs.append(hPr1);
            if(!allreadyPass.contains(hPr2) && !newGenHashs.contains(hPr2)) {
                result.append(pr2);
            }
            newGenHashs.append(hPr2);
        }

        i1 = (id / NB_ACCOUPLE);
        i2 = (i1 + 1 + id % NB_ACCOUPLE);
    }

    qDebug() << "Taille tmp 3" << result.size();

    // On mute en changeant les poids
    for(int i=0;i<NB_MUTE && i<generation.size();i++) {
        Reseau::Poids p = generation[i].poids;
        Reseau::Poids pr;
        int nbTest = 0;
        QString hpr;

        do {
            pr.clear();

            for(int j=0;j<p.size();j++) {
                QList<QList<float> > lpc = p[j];
                QList<QList<float> > lprc;

                for(int k=0;k<lpc.size();k++) {
                    QList<float> lpn = lpc[k];
                    QList<float> lprn;

                    for(int l=0;l<lpn.size();l++) {
                        int m = rand() % 9;
                        lprn.append(lpn[l] * (m >= 6 ? (1.0 + MUTE_STEP) : m >= 3 ? (1.0 - MUTE_STEP) : 1));
                    }

                    lprc.append(lprn);
                }

                pr.append(lprc);
            }

            hpr = getHash(pr);
        } while((allreadyPass.contains(hpr) || newGenHashs.contains(hpr)) && ++nbTest < MAX_TEST);

        result.append(pr);
        newGenHashs.append(hpr);
    }

    qDebug() << "Taille tmp 4" << result.size();

    // On mute en affectant de nouveau poids
    for(int i=0;i<NB_MUTE && i<generation.size();i++) {
        Reseau::Poids p = generation[i].poids;
        Reseau::Poids pr;
        int nbTest = 0;
        QString hpr;

        do {
            pr.clear();

            for(int j=0;j<p.size();j++) {
                QList<QList<float> > lpc = p[j];
                QList<QList<float> > lprc;

                for(int k=0;k<lpc.size();k++) {
                    QList<float> lpn = lpc[k];
                    QList<float> lprn;

                    for(int l=0;l<lpn.size();l++) {
                        float p = lpn[l];
                        if(rand() % 10 >= 8) {
                            p = Random::generePoid();
                        }
                        lprn.append(p);
                    }

                    lprc.append(lprn);
                }

                pr.append(lprc);
            }
            hpr = getHash(pr);
        }while((allreadyPass.contains(hpr) || newGenHashs.contains(hpr)) && ++nbTest < MAX_TEST);

        result.append(pr);
        newGenHashs.append(hpr);
    }

    // On supprime le surplus
    while(result.size() > SIZE_GENERATION) {
        result.removeLast();
    }

    qDebug() << "Nouvelle génération, création de" << result.size() << "individus";

    return result;
}

QString  MainWindow::getHash(const Reseau::Poids& poids) const {
    QString notHash = "";
    QString s = "";

    for(int j=0;j<poids.size();j++) {
        QList<QList<float> > lpc = poids[j];

        for(int k=0;k<lpc.size();k++) {
            QList<float> lpn = lpc[k];

            for(int l=0;l<lpn.size();l++) {
                 QString value = QString("%1").arg(lpn[l], 0, 'f', 2);
                 notHash += s+value;
                 s = "-";
            }
        }
    }

    return QString(QCryptographicHash::hash(notHash.toLatin1(), QCryptographicHash::Md5).toHex());
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

            allreadyPass.append(getHash(gr.poids));

            poids = Reseau::Poids();
            idx++;
            if(idx < newGeneration.size()) {
                poids = newGeneration[idx];
            }           
        } else {
            qDebug() << "Survivants :" << generation.size() << bestScoreGeneration << bestScore;
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
