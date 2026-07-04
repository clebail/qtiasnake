#ifndef SNAKEWIDGET_H
#define SNAKEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include "game.h"

class SnakeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SnakeWidget(QWidget *parent = nullptr);
    void setGame(const Game& game);
    // Rendu hors-ecran d'un etat de jeu, a taille fixe (pour l'export video).
    QImage renderFrame(const Game& game, int w, int h);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    Game game;

    void drawGame(QPainter *painter, const Game& game, int w, int h);
};

#endif // SNAKEWIDGET_H
