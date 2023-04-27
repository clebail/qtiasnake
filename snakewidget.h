#ifndef SNAKEWIDGET_H
#define SNAKEWIDGET_H

#include <QWidget>
#include <QPainter>
#include "game.h"

class SnakeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SnakeWidget(QWidget *parent = nullptr);
    void setGame(const Game& game);
    void setPasteque(const QPoint& pasteque);
    void showSensors(bool sensors);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    Game game;
    bool sensors;

    void drawGame(QPainter *painter);
};

#endif // SNAKEWIDGET_H
