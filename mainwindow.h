#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#define SIZE_GENERATION     1000
#define ELITE               5
#define NB_ACCOUPLE         30
#define NB_MUTE             30
#define TABOU_TIME          10

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

    void newGame(const Reseau::Poids& poids = Reseau::Poids());
    QList<Reseau::Poids> fusion(bool gardeElite = true) const;
    QString getHash(const Reseau::Poids& poids) const;

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
