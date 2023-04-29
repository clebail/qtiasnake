#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#define SIZE_GENERATION     500

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
    int bestScore;
    QList<Game::GameResult> generation;
    QList<Reseau::Poids> newGeneration;
    QTimer *timer;
    Reseau::Poids poids;

    void newGame(const Reseau::Poids& poids = Reseau::Poids());
    QList<Reseau::Poids> fusion() const;
    Reseau::Poids fusion(const Reseau::Poids &p1, const Reseau::Poids &p2) const;
    QList<QList<float> > fusion(const QList<QList<float> > &l1, const QList<QList<float> > &l2) const;
    QList<float> fusion(const QList<float> &l1, const QList<float> &l2) const;

private slots:
    void on_pbStep_clicked();
    void onTimer();
};
#endif // MAINWINDOW_H
