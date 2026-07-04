#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>
#include <QVector>
#include <QColor>

#define NB_DIRECTION            4

class Game {
public:
    typedef enum { edHaut = 0, edDroite = 1, edBas = 2, edGauche = 3 } Direction;

    Game(int largeur = 22, int hauteur = 22, QList<QPoint> pasteques = QList<QPoint>(), QList<QColor> snakeColors = QList<QColor>());

    int getLargeur() const;
    int getHauteur() const;
    const QList<QPoint>& getSnake() const;
    const QPoint& getPasteque() const;
    const QPoint& getQueue() const;

    Direction getDirection() const;
    void setDirection(const Direction& direction);   // le solveur choisit le prochain cap

    bool isOccupe(const QPoint& p) const;             // case occupée par le serpent
    bool isLibre(const QPoint& p) const;              // dans le plateau (hors murs) et libre

    bool step();                                      // avance d'un pas ; false si mort ou gagné
    bool estGagne() const;                            // le serpent remplit le plateau

    void getIncs(const Direction& direction, int& incX, int& incY) const;
    const QList<QPoint> getPasteques() const;
    void setPasteques(const QList<QPoint>& pasteques);
    void setSnakeColors(const QList<QColor>& snakeColors);
    const QList<QColor> getSnakeColors() const;
private:
    int largeur;
    int hauteur;
    QList<QPoint> snake;
    QVector<char> occupe;       // grille d'occupation du serpent (1 = case occupée)
    QPoint pasteque;
    Direction direction;
    bool gagne;
    QList<QPoint> pasteques;
    int currentPasteque;
    QList<QColor> snakeColors;

    int idx(const QPoint& p) const;
    QPoint newPasteque();
    void nextPasteque();
};

#endif // GAME_H
