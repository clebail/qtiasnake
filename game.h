#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>
#include <QVector>

#define NB_DIRECTION            4

class Game {
public:
    typedef enum { edHaut = 0, edDroite = 1, edBas = 2, edGauche = 3 } Direction;

    Game(int largeur = 22, int hauteur = 22);

    int getLargeur() const;
    int getHauteur() const;
    const QList<QPoint>& getSnake() const;
    const QPoint& getPasteque() const;

    Direction getDirection() const;
    void setDirection(const Direction& direction);   // le solveur choisit le prochain cap

    bool isOccupe(const QPoint& p) const;             // case occupée par le serpent
    bool isLibre(const QPoint& p) const;              // dans le plateau (hors murs) et libre

    bool step();                                      // avance d'un pas ; false si mort ou gagné
    bool estGagne() const;                            // le serpent remplit le plateau

    void getIncs(const Direction& direction, int& incX, int& incY) const;

private:
    int largeur;
    int hauteur;
    QList<QPoint> snake;
    QVector<char> occupe;       // grille d'occupation du serpent (1 = case occupée)
    QPoint pasteque;
    Direction direction;
    bool gagne;

    int idx(const QPoint& p) const;
    QPoint newPasteque() const;
    void nextPasteque();
};

#endif // GAME_H
