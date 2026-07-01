#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include "game.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Game game;
    QTimer *timer;
    bool showGame;

    void newGame();

private slots:
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void on_pbStep_clicked();
    void on_pbSetInterval_clicked();
    void on_cbShowGame_stateChanged(int);
    void onTimer();
};

#endif // MAINWINDOW_H
