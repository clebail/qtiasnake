#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#define SIZE_GENERATION     50

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:


    Game game;
    int idx;
    int bestScore;
    QList<Game::GameResult> generation;
    QTimer *timer;

    void newGame();
private slots:
    void on_pbStep_clicked();
    void onTimer();
};
#endif // MAINWINDOW_H
