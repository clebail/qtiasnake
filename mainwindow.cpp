#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFileDialog>
#include <QtDebug>
#include <QCryptographicHash>
#include <QDir>
#include <QImage>
#include <QPainter>
#include "mainwindow.h"
#include "snakewidget.h"
#include "random.h"

#define MAX_TEST                100

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(1);
    gameWidget->showSensors(false);

    idx = bestScoreVisite = idxGeneration = bestScoreVisiteGeneration = bestScorePasteque = bestScorePastequeGeneration = 0;
    evalRun = survived = 0;
    accScoreVisite = accScorePasteque = 0;
    showGame = true;

    // Mode capture vidéo : activé via QTIASNAKE_VIDEO=1 (non-intrusif — les runs
    // normaux ne sont pas affectés). Lance un entraînement neuf, affichage coupé
    // pour la vitesse, et s'arrête tout seul après videoGens générations.
    videoCapture = qEnvironmentVariableIsSet("QTIASNAKE_VIDEO");
    if(videoCapture) {
        videoGens       = qEnvironmentVariable("QTIASNAKE_VIDEO_GENS", "100").toInt();
        videoStride     = qMax(1, qEnvironmentVariable("QTIASNAKE_VIDEO_STRIDE", "1").toInt());
        videoMaxFrames  = qEnvironmentVariable("QTIASNAKE_VIDEO_MAXFRAMES", "100000").toInt();
        videoDir        = qEnvironmentVariable("QTIASNAKE_VIDEO_DIR", "frames");
        videoW = videoH = qEnvironmentVariable("QTIASNAKE_VIDEO_SIZE", "800").toInt();
        QDir().mkpath(videoDir);
        qDebug() << "Capture vidéo activée:" << videoGens << "gén, stride" << videoStride
                 << ", dossier" << videoDir << ", taille" << videoW;

        // On coupe l'affichage (vitesse) et on démarre l'entraînement seulement
        // après le newGame() initial ci-dessous, qui a peuplé gameWidget/reseauWidget
        // avec un réseau valide (sinon leur paintEvent peint du vide → crash).
        QTimer::singleShot(0, this, [this]() {
            showGame = false;
            cbShowGame->setChecked(false);
            on_pbStart_clicked();
        });
    }

    newGame();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame(const Reseau::Poids &poids) {
    int sGeneration = qMax(SIZE_GENERATION, newGeneration.size());
    game = Game(22, 22, poids);
    if(showGame) {
        gameWidget->setGame(game);
        reseauWidget->setReseau(game.getReseau());
    }

    lblGeneration->setText(QString().number(idxGeneration+1)+QString(" -- ")+QString().number(idx)+QString(" / ")+QString().number(sGeneration));
}

QList<Reseau::Poids> MainWindow::fusion(bool gardeElite) const {
    QList<Reseau::Poids> result;
    int i1 = 0;
    int i2 = 1;
    int id = 0;
    int nbNeurone = game.getNbNeurone();
    QString hash;
    QList<QString> newGenHashs;

    // Mutation décroissante : forte au début (exploration), douce ensuite
    // (réglage fin), interpolée linéairement sur MUTE_DECAY_GEN générations.
    float decay = qMin(1.0f, idxGeneration / (float)MUTE_DECAY_GEN);
    int muteRate = MUTE_RATE_START + decay * (MUTE_RATE_END - MUTE_RATE_START);
    float muteSigma = MUTE_SIGMA_START + decay * (MUTE_SIGMA_END - MUTE_SIGMA_START);
    qDebug() << "Mutation gén" << idxGeneration << "rate" << muteRate << "sigma" << muteSigma;

    // On prend l'élite tel quel
    if(gardeElite) {
        for(int i=0;i<ELITE && i<generation.size();i++) {
            result.append(generation[i].poids);

            newGenHashs.append(getHash(generation[i].poids));
        }

        qDebug() << "Taille elite" << result.size();
    }

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
            if(!tabou.contains(hPr1) && !newGenHashs.contains(hPr1)) {
                result.append(pr1);
            }
            newGenHashs.append(hPr1);
            if(!tabou.contains(hPr2) && !newGenHashs.contains(hPr2)) {
                result.append(pr2);
            }
            newGenHashs.append(hPr2);
        }

        i1 = (id / NB_ACCOUPLE);
        i2 = (i1 + 1 + id % NB_ACCOUPLE);
    }

    qDebug() << "Taille après accouplement 1" << result.size();

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
            if(!tabou.contains(hPr1) && !newGenHashs.contains(hPr1)) {
                result.append(pr1);
            }
            newGenHashs.append(hPr1);
            if(!tabou.contains(hPr2) && !newGenHashs.contains(hPr2)) {
                result.append(pr2);
            }
            newGenHashs.append(hPr2);
        }

        i1 = (id / NB_ACCOUPLE);
        i2 = (i1 + 1 + id % NB_ACCOUPLE);
    }

    qDebug() << "Taille après accouplement 2" << result.size();

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
                        float w = lpn[l];
                        if(rand() % 100 < muteRate) {
                            w += Random::muteNoise(muteSigma);
                        }
                        lprn.append(w);
                    }

                    lprc.append(lprn);
                }

                pr.append(lprc);
            }

            hpr = getHash(pr);
        } while((tabou.contains(hpr) || newGenHashs.contains(hpr)) && ++nbTest < MAX_TEST);

        result.append(pr);
        newGenHashs.append(hpr);
    }

    qDebug() << "Taille après mutaion 1" << result.size();

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
                        float w = lpn[l];
                        if(rand() % 100 < RESET_RATE) {
                            w = Random::generePoid();
                        }
                        lprn.append(w);
                    }

                    lprc.append(lprn);
                }

                pr.append(lprc);
            }
            hpr = getHash(pr);
        }while((tabou.contains(hpr) || newGenHashs.contains(hpr)) && ++nbTest < MAX_TEST);

        result.append(pr);
        newGenHashs.append(hpr);
    }

    qDebug() << "Taille après mutaion 2" << result.size();

    return result;
}

QString MainWindow::getHash(const Reseau::Poids& poids) const {
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

void MainWindow::organizeGeneration() {
    Game::ScoreType currentScoreType = Game::stVisite;
    int i, j;

    for(i=0;i<generation.size();i++) {
        int max = 0;
        int idx = 0;
        bool first = true;

        for(j=i;j<generation.size();j++) {
            if(first || generation[j].getScore(currentScoreType) > max) {
                max = generation[j].getScore(currentScoreType);
                idx = j;
                first = false;
            }
        }

        if(idx != i) {
            generation.insert(i, generation.takeAt(idx));
        }
        currentScoreType = (Game::ScoreType)(1 - (int)currentScoreType);
    }
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
    iterate();
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

        json["input_size"] = game.getReseau().getCouches().first().getNbEntree();
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

                        QList<Reseau::Poids> loaded;
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

                           loaded.append(poids);
                        }

                        // Validation d'architecture complète : le fichier doit
                        // correspondre au réseau courant couche par couche, avec une
                        // seule tolérance — 2 entrées ajoutées à la 1re couche depuis
                        // la sauvegarde (on migre alors avec des poids nuls).
                        Reseau::Poids target = game.getReseau().getPoids();
                        bool ok = !loaded.isEmpty();
                        bool migrate = false;

                        if(ok) {
                            const Reseau::Poids& s = loaded.first();
                            ok = (s.size() == target.size());
                            for(int j=0; ok && j<target.size(); j++) {
                                if(s[j].size() != target[j].size()) { ok = false; break; }  // nb neurones
                                int sw = s[j].isEmpty() ? 0 : s[j].first().size();
                                int tw = target[j].isEmpty() ? 0 : target[j].first().size();
                                if(j == 0 && sw == tw - 2) {
                                    migrate = true;                 // 2 entrées ajoutées
                                } else if(sw != tw) {
                                    ok = false;
                                }
                            }
                        }

                        if(!ok) {
                            qWarning() << "Import annulé : architecture incompatible avec le réseau courant";
                            return;
                        }

                        if(migrate) {
                            // Insère 2 poids nuls avant le biais de chaque neurone de
                            // la 1re couche → réseau identique, l'évolution apprendra
                            // ensuite à exploiter les nouvelles entrées.
                            for(int i=0;i<loaded.size();i++) {
                                for(int n=0;n<loaded[i][0].size();n++) {
                                    loaded[i][0][n].insert(loaded[i][0][n].size() - 1, 0.0f);
                                    loaded[i][0][n].insert(loaded[i][0][n].size() - 1, 0.0f);
                                }
                            }
                            qDebug() << "Migration population : +2 entrées (poids nuls) sur la 1re couche";
                        }

                        newGeneration = loaded;
                        idxGeneration = jsonObj["generation_idx"].toInt();
                        idx = 0;
                        evalRun = survived = 0;
                        accScoreVisite = accScorePasteque = 0;
                        Game::resetPasteques();
                        generation.clear();
                        poids = newGeneration.first();
                        newGame(poids);
                    }
                }
            }
        }
    }
}

void MainWindow::onTimer() {
    // Affichage actif : une itération par tic (on regarde jouer).
    // Affichage coupé : gros paquet par tic, on évite le coût de la boucle
    // d'événements Qt et des mises à jour d'UF à chaque pas.
    int budget = showGame ? 1 : STEPS_PAR_TICK;

    for(int i=0;i<budget;i++) {
        iterate();
        if(!timer->isActive())   // stoppé en cours (fin de génération, step-over)
            break;
    }
}

void MainWindow::iterate() {
    if(game.step()) {
        if(showGame) {
            gameWidget->setGame(game);
            reseauWidget->setReseau(game.getReseau());

            lblMvt->setText(QString().number(game.getTotMouvement())+QString(" -- ")+QString().number(game.getNbMouvement())+QString(" / ")+QString().number(game.getMaxMouvement()));
        }
    } else {
        if(over) {
            on_pbStop_clicked();
        }

        if(idx < qMax(SIZE_GENERATION, newGeneration.size())) {
            Game::GameResult gr = game.getResult();

            // On fige les poids réellement joués au 1er run : pour un individu
            // aléatoire (au-delà de newGeneration), newGame() en regénère sinon
            // un différent, et on moyennerait des réseaux distincts.
            if(evalRun == 0) {
                currentWeights = gr.poids;
            }

            accScoreVisite += gr.scoreVisite;
            accScorePasteque += gr.scorePasteque;
            if(!gr.perdu) {
                survived++;
            }

            evalRun++;
            if(evalRun < NB_EVAL) {
                newGame(currentWeights);   // même réseau, plateau différent
                return;
            }

            // NB_EVAL parties faites : on agrège (moyenne) avant de juger.
            Game::GameResult gr2;
            gr2.poids = currentWeights;
            gr2.scoreVisite = (int)(accScoreVisite / NB_EVAL);
            gr2.scorePasteque = (int)(accScorePasteque / NB_EVAL);
            gr2.perdu = (survived == 0);   // perdu seulement s'il meurt sur les NB_EVAL

            if(!gr2.perdu) {
                if(gr2.scoreVisite > bestScoreVisite) {
                    bestScoreVisite = gr2.scoreVisite;
                }
                if(gr2.scoreVisite > bestScoreVisiteGeneration) {
                    bestScoreVisiteGeneration = gr2.scoreVisite;
                }
                if(gr2.scorePasteque > bestScorePasteque) {
                    bestScorePasteque = gr2.scorePasteque;
                }
                if(gr2.scorePasteque > bestScorePastequeGeneration) {
                    bestScorePastequeGeneration = gr2.scorePasteque;
                }

                lblBestScore->setText(QString().number(bestScoreVisiteGeneration)+QString(" / ")+QString().number(bestScoreVisite)+QString(" -- ")+QString().number(bestScorePastequeGeneration)+QString(" / ")+QString().number(bestScorePasteque));

                generation.append(gr2);
            }

            tabou.insert(getHash(gr2.poids), TABOU_TIME);

            accScoreVisite = accScorePasteque = 0;
            survived = 0;
            evalRun = 0;

            poids = Reseau::Poids();
            idx++;
            if(idx < newGeneration.size()) {
                poids = newGeneration[idx];
            }
        } else {
            qDebug() << "Survivants :" << generation.size() << bestScoreVisiteGeneration << bestScoreVisite;
            Game::SortGameResultByVisite sorterVisite;
            Game::SortGameResultByPasteque sorterPasteque;

            std::sort(generation.begin(), generation.end(), sorterVisite);

            // Rejeu fidèle du meilleur : le cache de pastèques est encore intact
            // (resetPasteques() plus bas), donc on reproduit exactement la partie.
            if(videoCapture && idxGeneration < videoGens) {
                captureGeneration();
            }

            newGeneration = fusion();

            /*std::sort(generation.begin(), generation.end(), sorterPasteque);
            newGeneration.append(fusion());

            organizeGeneration();
            newGeneration.append(fusion(false));*/

            QMap<QString, int>::iterator i;
            QList<QString> toRemove;
            for(i=tabou.begin();i!=tabou.end();++i) {
                i.value()--;
                if(!i.value()) {
                    toRemove.append(i.key());
                }
            }

            foreach(auto k, toRemove) {
                tabou.remove(k);
            }

            generation.clear();
            idx = 0;
            evalRun = survived = 0;
            accScoreVisite = accScorePasteque = 0;
            idxGeneration++;
            Game::resetPasteques();   // nouvelle séquence de pastèques pour la génération suivante
            if(newGeneration.size()) poids = newGeneration[idx];
            bestScoreVisiteGeneration = bestScorePastequeGeneration = 0;

            lblBestScore->setText(QString().number(bestScoreVisiteGeneration)+QString(" / ")+QString().number(bestScoreVisite));

            if(cbStopGen->checkState() == Qt::CheckState::Checked) {
                on_pbStop_clicked();
            }

            if(videoCapture && idxGeneration >= videoGens) {
                on_pbStop_clicked();
                qDebug() << "Capture vidéo terminée:" << videoGens
                         << "générations dans" << videoDir;
            }
        }

        newGame(poids);
    }
}

void MainWindow::captureGeneration() {
    if(generation.isEmpty()) {
        return;
    }

    const Game::GameResult& best = generation.first();
    int gen = idxGeneration + 1;   // libellé 1-based

    // Rejeu sur le cache de pastèques encore intact → partie identique à celle
    // qui a produit best.scoreVisite / best.scorePasteque en entraînement.
    Game replay(22, 22, best.poids);

    int step = 0;
    int frame = 0;
    while(true) {
        if(step % videoStride == 0) {
            saveFrame(replay, gen, frame++, best.scorePasteque);
            if(frame >= videoMaxFrames) {
                break;
            }
        }
        step++;
        if(!replay.step()) {
            break;
        }
    }

    qDebug() << "Gén" << gen << ": rejeu" << step << "pas ->" << frame << "frames"
             << "(record" << best.scorePasteque << "pastèques)";
}

void MainWindow::saveFrame(Game& g, int gen, int frame, int bestPasteque) {
    QImage image(videoW, videoH, QImage::Format_RGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setPen(QColorConstants::Black);
    painter.setBrush(QColorConstants::White);
    painter.drawRect(QRect(0, 0, videoW - 1, videoH - 1));

    // Stats courantes du rejeu + record de la génération.
    int pasteques = g.getSnake().size() - 4;
    int longueur = g.getSnake().size();
    QString overlay = QString("Génération %1").arg(gen)
        + "\n"
        + QString("Pastèques: %1   •   Longueur: %2   •   Mvts: %3   •   Record: %4")
              .arg(pasteques).arg(longueur).arg(g.getTotMouvement()).arg(bestPasteque);

    SnakeWidget::renderGame(&painter, g, videoW, videoH, false, overlay);
    painter.end();

    // Zéro-padding : le tri lexicographique du glob ffmpeg donne l'ordre chrono
    // (gen001_frame00000 < gen001_frame00001 < ... < gen002_frame00000).
    QString name = QString("%1/gen%2_frame%3.png")
        .arg(videoDir)
        .arg(gen, 3, 10, QChar('0'))
        .arg(frame, 5, 10, QChar('0'));
    image.save(name);
}
