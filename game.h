#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>
#include "sensor.h"
#include "reseau.h"

class Game {
public:
    typedef enum { edHaut, edDroite, edBas, edGauche } Direction;

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
    int getTotMouvement() const;
    const Reseau& getReseau() const;
    void setReseau(const Reseau& reseau);
    Game::GameResult getResult() const;
    int getNbCaseVisite() const;
private:
    int largeur;
    int hauteur;
    QList<QPoint> snake;
    QList<Sensor> sensors;
    QPoint pasteque;
    int nbMouvement;
    int totMouvement;
    Reseau reseau;
    float diagonale;
    QList<int> caseVisite;
    int idPasteque;
    bool perdu;
    Game::Direction direction;
    Game::Direction queueDirection;
    int maxMouvement;

    Sensor::ESensorType cellFree(const QPoint& p, const Sensor::ESensorType& toIgnore = Sensor::estNone) const;
    void calculSensors();
    Sensor getFirstCellOccupe(int incX, int incY, const Sensor::ESensorType& toIgnore = Sensor::estNone) const;
    void addSensorsForDirection(int incX, int incY);
    void newPasteque();
    void next();
    void initReseau();
    void getIncs(const Direction &direction, int &incX, int &incY) const;
    Game::Direction calculDirection(int incX, int incY);
};

#endif // GAME_H
