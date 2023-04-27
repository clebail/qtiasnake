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
    const QList<QPoint>& getSensors() const;
    const QPoint& getPasteque() const;
    void step(int incX, int incY);
private:
    int largeur;
    int hauteur;
    QList<QPoint> snake;
    QList<QPoint> sensors;
    QPoint pasteque;

    bool cellFree(const QPoint& p) const;
    void calculSensors();
    QPoint getFirstCellOccupe(int incX, int incY) const;
};

#endif // GAME_H
