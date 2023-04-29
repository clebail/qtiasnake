#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>
#include "sensor.h"
#include "reseau.h"

class Game {
public:
    typedef struct _GameResult {
        Reseau::Poids poids;
        int score;       
    } GameResult;

    typedef struct _SortGameResult {
        bool operator()(Game::GameResult a, Game::GameResult b) const { return a.score > b.score; }
    } SortGameResult;

    Game(int largeur = 12, int hauteur = 12, const Reseau::Poids& poids = Reseau::Poids());
    int getLargeur();
    int getHauteur();
    const QList<QPoint>& getSnake() const;
    const QList<Sensor> &getSensors() const;
    const QPoint& getPasteque() const;
    bool step();
    int getNbMouvement() const;
    int getIncX() const;
    int getIncY() const;
    const Reseau& getReseau() const;
    void setReseau(const Reseau& reseau);
    Game::GameResult getResult() const;
private:
    int largeur;
    int hauteur;
    QList<QPoint> snake;
    QList<Sensor> sensors;
    QPoint pasteque;
    int nbMouvement;
    int totMouvement;
    int incX;
    int incY;
    Reseau reseau;
    float diagonale;
    QList<int> caseVisite;
    int lastSortie;

    Sensor::ESensorType cellFree(const QPoint& p, bool ignoreFood = false) const;
    void calculSensors();
    Sensor getFirstCellOccupe(int incX, int incY) const;
    void newPasteque();
    void next();
    void initReseau();
};

#endif // GAME_H
