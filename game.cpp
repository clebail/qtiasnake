#include <QtDebug>
#include <math.h>
#include "game.h"

#define NB_SORTIE           4
#define PI                  ((float)(3.14159))

Game::Game(int largeur, int hauteur, const Reseau::Poids &poids) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    maxMouvement = (largeur - 2) * (hauteur - 2);

    snake.append(QPoint(largeur/2, hauteur/2));
    nbMouvement = totMouvement = 0;

    direction = Game::edHaut;
    queueDirection = Game::edHaut;

    nextPasteque();

    calculSensors();

    if(poids.size() == 0) {
        initReseau();
    } else {
        reseau = Reseau(poids);
    }

    caseVisite.clear();
    caseVisite.append(snake[0].x() + snake[0].y() * largeur);

    perdu = false;
}

int Game::getLargeur() {
    return largeur;
}

int Game::getHauteur() {
    return hauteur;
}

const QList<QPoint>& Game::getSnake() const {
    return snake;
}

const QList<Sensor>&  Game::getSensors() const {
    return sensors;
}

const QPoint& Game::getPasteque() const {
    return pasteque;
}

bool Game::step() {
    int i;
    QPoint last = snake.last();
    QPoint newTete;
    Sensor::ESensorType type;
    int idCase;
    int incX, incY;

    getIncs(direction, incX, incY);
    newTete.setX(snake[0].x() + incX);
    newTete.setY(snake[0].y() + incY);

    idCase = newTete.x() + newTete.y() * largeur;

    type = cellFree(newTete);
    if(type != Sensor::estMur && type != Sensor::estSnake)
    {
        bool first = true;
        for(i=snake.size()-1;i>=1;i--) {
            if(first) {
                incX = snake[i-1].x() - snake[i].x();
                incY = snake[i-1].y() - snake[i].y();
                queueDirection = calculDirection(incX, incY);
            }
            snake[i].setX(snake[i-1].x());
            snake[i].setY(snake[i-1].y());
            first = false;
        }

        snake[0] = newTete;
        nbMouvement++;       

        if(type == Sensor::estPasteque) {
            snake.append(last);

            nextPasteque();
            nbMouvement = 0;

            caseVisite.clear();
        }

        if(!caseVisite.contains(idCase)) {
            caseVisite.append(idCase);
            totMouvement++;
        }

        if(nbMouvement == maxMouvement) {
            return false;
        }

        calculSensors();
        next();

        return true;

    }

    perdu = true;

    return false;
}

int Game::getTotMouvement() const {
    return totMouvement;
}

int Game::getNbMouvement() const {
    return nbMouvement;
}

int Game::getMaxMouvement() const {
    return maxMouvement;
}

const Reseau& Game::getReseau() const {
    return reseau;
}

void Game::setReseau(const Reseau& reseau) {
    this->reseau = reseau;
}

Game::GameResult Game::getResult() const {
    Game::GameResult gr;

    gr.poids = reseau.getPoids();
    gr.score = (totMouvement + (snake.size() - 1) * 1000) * (perdu ? 0 : 1);
    // gr.score = (1 + (snake.size() - 1) * 1000) * (perdu ? 0 : 1);

    return gr;
}

int Game::getNbCaseVisite() const {
    return caseVisite.size();
}

QList<QPoint> Game::getPasteques() const {
    return pasteques;
}

Sensor::ESensorType Game::cellFree(const QPoint& p, const Sensor::ESensorType &toIgnore) const {
    int x = p.x();
    int y = p.y();

    if(x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1) {
        QList<QPoint>::const_iterator i;

        if(toIgnore != Sensor::estSnake) {
            for(i=snake.begin();i!=snake.end();++i) {
                if((*i).x() == x && (*i).y() == y) {
                    return Sensor::estSnake;
                }
            }
        }

        return toIgnore != Sensor::estPasteque && p == pasteque ? Sensor::estPasteque : Sensor::estNone;
    }

    return Sensor::estMur;
}

void Game::calculSensors() {
   sensors.clear();

    /*switch(direction) {
    case Game::edHaut:*/
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
    /*    break;
    case Game::edDroite:
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        break;
    case Game::edBas:
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        break;
    case Game::edGauche:
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        break;
    }*/
}

Sensor Game::getFirstCellOccupe(int incX, int incY, const Sensor::ESensorType& toIgnore) const {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;

    p = QPoint(tete.x() + incX, tete.y() +incY);
    while((type = cellFree(p, toIgnore)) == Sensor::estNone) {
       p = QPoint(p.x() + incX, p.y() +incY);
    }

    return Sensor(p, type);
}

void Game::addSensorsForDirection(int incX, int incY) {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;
    QMap<Sensor::ESensorType, QPoint> tmp;

    p = QPoint(tete.x() + incX, tete.y() + incY);
    while((type = cellFree(p)) != Sensor::estMur) {
        if(type != Sensor::estSnake || !tmp.contains(Sensor::estSnake)) {
            tmp.insert(type, p);
        }
        p = QPoint(p.x() + incX, p.y() +incY);
    }

    sensors.append(Sensor(p, Sensor::estMur));

    if(tmp.contains(Sensor::estSnake)) {
        sensors.append(Sensor(tmp.value(Sensor::estSnake), Sensor::estSnake));
    } else {
        sensors.append(Sensor(QPoint(), Sensor::estSnake));
    }

    if(tmp.contains(Sensor::estPasteque)) {
        sensors.append(Sensor(tmp.value(Sensor::estPasteque), Sensor::estPasteque));
    } else {
        sensors.append(Sensor(QPoint(), Sensor::estPasteque));
    }
}

QPoint Game::newPasteque() const {
    QPoint p;
    do {
        p = QPoint(rand() % (largeur - 2) + 1, rand() % (hauteur - 2) + 1);
    }while (cellFree(p) != Sensor::estNone);

    return p;
}

void Game::nextPasteque() {
    pasteque = newPasteque();
}

void Game::next() {
    QList<float> entrees;
    QList<float> sorties = reseau.getSorties();
    QPoint tete = snake[0];
    int incX, incY;
    int queueIncX, queueIncY;
    int incS = 0;
    int newDir;
    int goodSortie = 0;

    getIncs(direction, incX, incY);
    getIncs(queueDirection, queueIncX, queueIncY);

    for(int i=0;i<sensors.size();i++) {
        QPoint p = sensors[i].getPoint();

        if(!p.isNull()) {
            int diffX = abs(p.x() - tete.x());
            int diffY = abs(p.y() - tete.y());

            if(diffX == 0) {
                entrees.append(diffY);
            } else if(diffY == 0) {
                entrees.append(diffX);
            } else {
                entrees.append(sqrt((float)(diffX * diffX + diffY * diffY)));
            }
        } else {
            entrees.append(0);
        }
    }


    entrees.append(incX);
    entrees.append(incY);
    entrees.append(queueIncX);
    entrees.append(queueIncY);

    sorties = reseau.eval(entrees);
    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] >= SEUIL) {
            goodSortie = i;
        }
    }

    if(goodSortie == 1) {
        incS = -1;
    } else if(goodSortie == 2) {
        incS = 1;
    }

    newDir = (int)direction + incS;
    if(newDir == -1) {
        newDir = NB_DIRECTION - 1;
    }
    if(newDir == NB_DIRECTION) {
        newDir = 0;
    }

    direction = (Game::Direction)newDir;

    if(snake.size() == 1) {
        queueDirection = direction;
    }
}

void Game::initReseau() {
    reseau.addCouche(Couche(28, 28));
    reseau.addCouche(Couche(28, 14));
    reseau.addCouche(Couche(14, 7));
    reseau.addCouche(Couche(7, 3));
}

void Game::getIncs(const Game::Direction& direction, int &incX, int &incY) const {
    switch(direction) {
    case Game::edHaut:
        incX = 0; incY = -1;
        return;
    case Game::edDroite:
        incX = 1; incY = 0;
        return;
    case Game::edBas:
        incX = 0; incY = 1;
        return;
    case Game::edGauche:
        incX = -1; incY = 0;
        return;
    }
}

Game::Direction Game::calculDirection(int incX, int incY) {
    if(incX == 0 && incY == -1) {
        return Game::edHaut;
    } else if(incX == 1 && incY == 0) {
        return Game::edDroite;
    } else if(incX == 0 && incY == 1) {
        return Game::edBas;
    } else {
        return Game::edGauche;
    }
}




