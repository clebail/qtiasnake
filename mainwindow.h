#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    QList<Game::GameResult> generation;

    void newGame();
private slots:
    void on_pbStep_clicked();
};
#endif // MAINWINDOW_H
