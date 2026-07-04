#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QtDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(spInterval->value());

    showGame = true;
    saveFrames = false;
    createImageMode = false;

    newGame(22, 22);
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::newGame(int largeur, int hauteur, QList<QPoint> pasteques, QList<QColor> snakeColors) {
    game = Game(largeur, hauteur, pasteques, snakeColors);
    solver = Solver(&game);
    stepCount = 0;
    frameIndex = 0;   // repart de zero : les frames de la partie precedente seront ecrasees

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
        if(saveFrames) {
            saveFrame();
        }
        updateCounters();

        timer->stop();
        pbStep->setEnabled(true);

        if(createImageMode && !image.isNull()) {
            QList<QPoint> snake = game.getSnake();
            QList<QColor> snakeColors;
            for(int i=0;i<snake.size();i++) {
                int x = snake.at(i).x()-1;
                int y = snake.at(i).y()-1;

                snakeColors.append(image.pixelColor(x, y));
            }

            saveGame(game.getPasteques(), snakeColors);

            QMessageBox::information(this, "Jeu sauvé", QString("Jeu sauvegardé"));
        } else {
            int pasteques = game.getSnake().size() - 4;
            QMessageBox::information(this, "Victoire",
                QString("Termine ! %1 pasteques mangees en %2 steps.")
                    .arg(pasteques).arg(stepCount));
        }
        return;
    }

    if(alive) {
        stepCount++;
        if(showGame) {
            gameWidget->setGame(game);
        }
        if(saveFrames) {
            saveFrame();
        }
        updateCounters();
    } else {
        // Mort : on fige l'état au lieu de redémarrer, pour inspecter le cas.
        if(showGame) {
            gameWidget->setGame(game);
        }
        if(saveFrames) {
            saveFrame();
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

void MainWindow::on_cbSaveFrames_stateChanged(int) {
    saveFrames = cbSaveFrames->checkState() == Qt::CheckState::Checked;
}

void MainWindow::on_actLoadImage_triggered() {
    fileName = QFileDialog::getOpenFileName(this, "Fichier image", QString(), "Images (*.png *.jpg)");

    if(!fileName.isEmpty()) {
        image = QImage(fileName);

        newGame(image.size().width()+2, image.size().height()+2);
        createImageMode = true;
    }
}

void MainWindow::on_actLoadGame_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Fichier de données de jeu", QString(), "Données de jeu (*.sna)");

    if(!fileName.isEmpty()) {
        GameData gameData = loadGame(fileName);

        newGame(gameData.size.width()+2, gameData.size.height()+2, gameData.pasteques, gameData.snakeColors);
    }
}

bool MainWindow::saveGame(const QList<QPoint>& pasteques, const QList<QColor>& snakeColors) const {
    QFile file(fileName + ".sna");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    out << image.size() << pasteques << snakeColors;

    return file.error() == QFile::NoError;
}

const MainWindow::GameData  MainWindow::loadGame(const QString& fileName) const{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        GameData gameData;
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_15);

        in >> gameData.size >> gameData.pasteques >> gameData.snakeColors;

        if(in.status() == QDataStream::Ok) {
            return gameData;
        }
    }

    return GameData();
}

void MainWindow::saveFrame() {
    // Au premier frame de la partie : (re)crée un dossier "frames" propre.
    if(frameIndex == 0) {
        QDir("frames").removeRecursively();
        QDir().mkpath("frames");
    }

    // Rendu hors-ecran a taille fixe (independant de l'affichage) : 32 px / case,
    // dimensions paires -> compatible ffmpeg/yuv420p.
    const int cell = 32;
    QImage img = gameWidget->renderFrame(game, game.getLargeur() * cell, game.getHauteur() * cell);

    // Une image par step, numerotee sur 6 chiffres -> assemblable par ffmpeg.
    img.save(QString("frames/frame_%1.png").arg(frameIndex, 6, 10, QChar('0')));

    frameIndex++;
}
