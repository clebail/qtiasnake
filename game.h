#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>

class Game
{
public:
    Game(int largeur = 12, int hauteur = 12);
    int getLargeur();
    int getHauteur();

    const QList<QPoint>& getSnake() const;
    const QPoint& getPasteque() const;
private:
    int largeur;
    int hauteur;

    QList<QPoint> snake;
    QPoint pasteque;
};

#endif // GAME_H
