#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Game game;
private slots:
    void on_pbXp_clicked();
    void on_pbXm_clicked();
    void on_pbYp_clicked();
    void on_pbYm_clicked();
};
#endif // MAINWINDOW_H
