#include <QtDebug>
#include <math.h>
#include "game.h"

#define NB_SORTIE           4
#define PI                  ((float)(3.14159))

Game::Game(int largeur, int hauteur, const Reseau::Poids &poids) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    diagonale = sqrt((largeur - 2) * (largeur - 2) + (hauteur - 2) * (hauteur - 2));

    snake.append(QPoint(3, hauteur - 5));
    snake.append(QPoint(3, hauteur - 4));
    snake.append(QPoint(3, hauteur - 3));
    snake.append(QPoint(3, hauteur - 2));
    nbMouvement = totMouvement = 0;

    pasteques.append(QPoint(4, 4));
    pasteques.append(QPoint(7, 7));
    pasteques.append(QPoint(7, 4));
    pasteques.append(QPoint(4, 7));
    pasteques.append(QPoint(3, 3));
    pasteques.append(QPoint(6, 6));
    pasteques.append(QPoint(6, 3));
    pasteques.append(QPoint(3, 6));

    direction = Game::edDroite;
    queueDirection = Game::edHaut;

    idPasteque = -1;

    newPasteque();

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
        totMouvement++;

        if(type == Sensor::estPasteque) {
            snake.append(last);

            newPasteque();
            nbMouvement = 0;
        }

        if(!caseVisite.contains(idCase)) {
            caseVisite.append(idCase);
        }

        if(nbMouvement == 100) {
            return false;
        }

        calculSensors();
        next();

        return true;

    }

    perdu = true;

    return false;
}

int Game::getNbMouvement() const {
    return totMouvement;
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
    // gr.score = pow(totMouvement, snake.size());
    // gr.score = pow(caseVisite.size(), snake.size());
    // gr.score = (snake.size() - 1) * 10 - (perdu ? 10 : 0);
    gr.score = (totMouvement * caseVisite.size() + (snake.size() - 4) * 10000) * (perdu ? 0 : 1);
    // gr.score = (snake.size() - 4) * 100;

    return gr;
}

int Game::getNbCaseVisite() const {
    return caseVisite.size();
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
    QPoint tete = snake[0];

    switch(direction) {
    case Game::edHaut:
        sensors.append(Sensor(QPoint(0, tete.y()), Sensor::estMur));
        sensors.append(Sensor(QPoint(qMax(0, tete.x() - tete.y()), qMax(0, tete.y()- tete.x())), Sensor::estMur));
        sensors.append(Sensor(QPoint(tete.x(), 0), Sensor::estMur));
        sensors.append(Sensor(QPoint(qMin(largeur - 1, tete.x() + tete.y()), qMax(0, tete.y() - largeur - tete.x() - 1)), Sensor::estMur));
        sensors.append(Sensor(QPoint(largeur - 1, tete.y()), Sensor::estMur));
        break;
    case Game::edDroite:
        sensors.append(Sensor(QPoint(tete.x(), 0), Sensor::estMur));
        sensors.append(Sensor(QPoint(qMin(largeur - 1, tete.x() + tete.y()), qMax(0, tete.y() - largeur + tete.x() + 1)), Sensor::estMur));
        sensors.append(Sensor(QPoint(largeur - 1, tete.y()), Sensor::estMur));
        sensors.append(Sensor(QPoint(largeur-1, hauteur-1), Sensor::estMur));
        sensors.append(Sensor(QPoint(hauteur - 1, tete.y()), Sensor::estMur));
        break;
    case Game::edBas:
        sensors.append(getFirstCellOccupe(0, 1));
        sensors.append(getFirstCellOccupe(-1, 0));
        sensors.append(getFirstCellOccupe(1, 0));

        sensors.append(getFirstCellOccupe(-1, 1));
        sensors.append(getFirstCellOccupe(1, 1));
        break;
    case Game::edGauche:
        sensors.append(getFirstCellOccupe(-1, 0));
        sensors.append(getFirstCellOccupe(0, -1));
        sensors.append(getFirstCellOccupe(0, 1));

        sensors.append(getFirstCellOccupe(-1, -1));
        sensors.append(getFirstCellOccupe(-1, 1));
        break;
    }

    sensors.append(Sensor(pasteque, Sensor::estPasteque));
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

void Game::newPasteque() {
    QPoint p;
    do {
        p = QPoint(rand() % (largeur - 2) + 1, rand() % (hauteur - 2) + 1);
        //idPasteque = (idPasteque + 1) % pasteques.size();
        // pasteque = pasteques[idPasteque];
    }while (cellFree(p) != Sensor::estNone);

    pasteque = p;
}

void Game::next() {
    QList<float> entrees;
    QList<float> sorties = reseau.getSorties();
    QPoint tete = snake[0];
    float max = -1;
    int incX, incY;
    int queueIncX, queueIncY;
    int goodSortie = 0;

    getIncs(direction, incX, incY);
    getIncs(queueDirection, queueIncX, queueIncY);

    for(int i=0;i<sensors.size();i++) {
        float diffX = (float)abs(sensors[i].getPoint().x() - tete.x());
        float diffY = (float)abs(sensors[i].getPoint().y() - tete.y());

        if(diffX == 0.0) {
            entrees.append(diffY);
        } else if(diffY == 0.0) {
            entrees.append(diffX);
        } else {
            entrees.append(sqrt(diffX * diffX + diffY * diffY));
        }
    }


    entrees.append(incX);
    entrees.append(incY);
    entrees.append(queueIncX);
    entrees.append(queueIncY);

    sorties = reseau.eval(entrees);

    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] > max && sorties[i] >= SEUIL) {
            goodSortie = i;
            max = sorties[i];
        }
    }

    /*if(goodSortie >= 1) {
        if(incX != 0) {
            incY = (goodSortie == 1 ? -1 : 1) * incX;
            incX = 0;
        } else {
            incX = (goodSortie == 1 ? 1 : -1) * incY;
            incY = 0;
        }

        direction = calculDirection(incX, incY);
        if(snake.size() == 1) {
            queueDirection = direction;
        }
    }*/
}

void Game::initReseau() {
    reseau.addCouche(Couche(10, 10));
    reseau.addCouche(Couche(10, 30));
    reseau.addCouche(Couche(30, 30));
    reseau.addCouche(Couche(30, 10));
    reseau.addCouche(Couche(10, 3));
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




