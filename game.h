#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QMap>
#include <QPoint>
#include "sensor.h"
#include "reseau.h"

#define NB_DIRECTION            4
#define LIMITE_VISITE           4

class Game {
public:
    typedef enum { edHaut = 0, edDroite = 1, edBas = 2, edGauche = 3 } Direction;
    typedef enum { stVisite = 0, stPasteque = 1 } ScoreType;

    typedef struct _GameResult {
        Reseau::Poids poids;
        int scoreVisite;
        int scorePasteque;
        bool perdu;

        int getScore(const ScoreType& scoreType) const {
            if(scoreType == stVisite) {
                return scoreVisite;
            }
            return scorePasteque;
        }
    } GameResult;

    typedef struct _SortGameResultByVisite {
        bool operator() (Game::GameResult a, Game::GameResult b) const { return a.scoreVisite > b.scoreVisite; }
    } SortGameResultByVisite;

    typedef struct _SortGameResultByPasteque {
        bool operator() (Game::GameResult a, Game::GameResult b) const { return a.scorePasteque > b.scorePasteque; }
    } SortGameResultByPasteque;

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
    QMap<int, int> getCaseVisite() const;
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
    QMap<int, int> caseVisite;
    bool perdu;
    Game::Direction direction;
    Game::Direction queueDirection;
    int maxMouvement;
    int idPasteque;
    int nbCgtDir;
    bool turnRight, turnLeft;
    int turns[2];
    int limiteVisite;

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
