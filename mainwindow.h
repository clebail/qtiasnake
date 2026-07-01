#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#define SIZE_GENERATION     1000
#define ELITE               5
#define NB_ACCOUPLE         75
#define NB_MUTE             75
#define TABOU_TIME          10
#define NB_EVAL             1
#define STEPS_PAR_TICK      3000    // pas traités par tic quand l'affichage est coupé

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Game game;
    int idx;
    int idxGeneration;
    int bestScoreVisite;
    int bestScoreVisiteGeneration;
    int bestScorePasteque;
    int bestScorePastequeGeneration;
    bool showGame;
    QList<Game::GameResult> generation;
    QList<Reseau::Poids> newGeneration;
    QTimer *timer;
    Reseau::Poids poids;
    bool over = false;
    QMap<QString, int> tabou;

    // Évaluation de chaque individu sur NB_EVAL parties (anti-bruit).
    int evalRun;
    int survived;
    qint64 accScoreVisite;
    qint64 accScorePasteque;
    Reseau::Poids currentWeights;

    void newGame(const Reseau::Poids& poids = Reseau::Poids());
    void iterate();
    QList<Reseau::Poids> fusion(bool gardeElite = true) const;
    QString getHash(const Reseau::Poids& poids) const;
    void organizeGeneration();

    // Capture vidéo (déclenchée par variables d'environnement) : rejeu fidèle
    // du meilleur individu de chaque génération en frames PNG offscreen.
    bool videoCapture = false;
    int videoGens = 100;        // nombre de générations à capturer
    int videoStride = 1;        // 1 frame sauvée tous les N pas
    int videoMaxFrames = 100000;// cap de frames par génération
    int videoW = 800, videoH = 800;
    QString videoDir = "frames";
    void captureGeneration();
    void saveFrame(Game& g, int gen, int frame, int bestPasteque);

private slots:
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void on_pbStep_clicked();
    void on_pbStepOver_clicked();
    void on_pbSetInterval_clicked();
    void on_cbSensors_stateChanged(int);
    void on_cbShowGame_stateChanged(int);
    void on_pbSave_clicked();
    void on_pbLoad_clicked();
    void onTimer();
};
#endif // MAINWINDOW_H
