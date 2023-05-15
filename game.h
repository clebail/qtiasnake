#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QPoint>
#include "sensor.h"
#include "reseau.h"

#define NB_DIRECTION            4

class Game {
public:
    typedef enum { edHaut = 0, edDroite = 1, edBas = 2, edGauche = 3 } Direction;

    typedef struct _GameResult {
        Reseau::Poids poids;
        int score;
        bool perdu;
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
    int getNbMouvement() const;
    int getMaxMouvement() const;
    const Reseau& getReseau() const;
    void setReseau(const Reseau& reseau);
    Game::GameResult getResult() const;
    int getNbCaseVisite() const;
    int getNbNeurone() const;
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
    bool perdu;
    Game::Direction direction;
    Game::Direction queueDirection;
    int maxMouvement;
    int idPasteque;
    int nbCgtDir;

    Sensor::ESensorType cellFree(const QPoint& p, const Sensor::ESensorType& toIgnore = Sensor::estNone) const;
    void calculSensors();
    Sensor getFirstCellOccupe(int incX, int incY, const Sensor::ESensorType& toIgnore = Sensor::estNone) const;
    void addSensorsForDirection(int incX, int incY);
    QPoint newPasteque() const;
    void nextPasteque();
    void next();
    void initReseau();
    void getIncs(const Direction &direction, int &incX, int &incY) const;
    Game::Direction calculDirection(int incX, int incY);
};

#endif // GAME_H
