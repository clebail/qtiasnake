#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include "game.h"
#include "solver.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    typedef struct _GameData {
        QSize size;
        QList<QPoint> pasteques;
        QList<QColor> snakeColors;
    } GameData;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Game game;
    Solver solver;
    QTimer *timer;
    bool showGame;
    bool saveFrames;
    int stepCount;
    QString fileName;
    bool createImageMode;
    QImage image;
    int frameIndex;

    void newGame(int largeur, int hauteur, QList<QPoint> pasteques = QList<QPoint>(), QList<QColor> snakeColors = QList<QColor>());
    void updateCounters();
    void saveFrame();
    bool saveGame(const QList<QPoint>& pasteques, const QList<QColor>& snakeColors) const;
    const MainWindow::GameData loadGame(const QString& fileName) const;
private slots:
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void on_pbStep_clicked();
    void on_pbSetInterval_clicked();
    void on_cbShowGame_stateChanged(int);
    void on_cbSaveFrames_stateChanged(int);
    void onTimer();
    void on_actLoadImage_triggered();
    void on_actLoadGame_triggered();
};

#endif // MAINWINDOW_H
