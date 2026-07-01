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

    // Rendu réutilisable : peint un plateau sur n'importe quel QPainter aux
    // dimensions données (widget visible OU QImage offscreen). Un overlay
    // multi-lignes optionnel ('\n' = 1re ligne titre, suivantes = détail).
    static void renderGame(QPainter *painter, Game& game, int w, int h,
                           bool showSensors, const QString& overlay = QString());

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    Game game;
    bool sensors;

    void drawGame(QPainter *painter);
};

#endif // SNAKEWIDGET_H
