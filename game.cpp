#include <QtDebug>
#include <math.h>
#include "game.h"

#define NB_SORTIE           4
#define PI                  ((float)(3.14159))

Game::Game(int largeur, int hauteur, const Reseau::Poids &poids) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    diagonale = sqrt((largeur - 2) * (largeur - 2) + (hauteur - 2) * (hauteur - 2));

    snake.append(QPoint(largeur/2, hauteur/2));
    nbMouvement = totMouvement = 0;

    pasteques.append(QPoint(4, 4));
    pasteques.append(QPoint(7, 7));
    pasteques.append(QPoint(7, 4));
    pasteques.append(QPoint(4, 7));
    pasteques.append(QPoint(3, 3));
    pasteques.append(QPoint(6, 6));
    pasteques.append(QPoint(6, 3));
    pasteques.append(QPoint(3, 6));

    direction = Game::edHaut;

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

    lastSortie = 0.0;

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

    getIncs(incX, incY);
    newTete.setX(snake[0].x() + incX);
    newTete.setY(snake[0].y() + incY);

    idCase = newTete.x() + newTete.y() * largeur;

    type = cellFree(newTete);
    if(type != Sensor::estMur && type != Sensor::estSnake)
    {
        for(i=snake.size()-1;i>=1;i--) {
            snake[i].setX(snake[i-1].x());
            snake[i].setY(snake[i-1].y());
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
    gr.score = caseVisite.size() + (snake.size() - 1) * 100 - (perdu ? 90 : 0);
    return gr;
}

Sensor::ESensorType Game::cellFree(const QPoint& p, bool ignoreFood) const {
    int x = p.x();
    int y = p.y();

    if(x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1) {
        QList<QPoint>::const_iterator i;

        for(i=snake.begin();i!=snake.end();++i) {
            if((*i).x() == x && (*i).y() == y) {
                return Sensor::estSnake;
            }
        }

        return !ignoreFood && p == pasteque ? Sensor::estPasteque : Sensor::estNone;
    }

    return Sensor::estMur;
}

void Game::calculSensors() {
    sensors.clear();

    switch(direction) {
    case Game::edHaut:
        sensors.append(getFirstCellOccupe(0, -1));
        sensors.append(getFirstCellOccupe(1, 0));
        sensors.append(getFirstCellOccupe(0, 1));
        sensors.append(getFirstCellOccupe(-1, 0));
        break;
    case Game::edDroite:
        sensors.append(getFirstCellOccupe(1, 0));
        sensors.append(getFirstCellOccupe(0, 1));
        sensors.append(getFirstCellOccupe(-1, 0));
        sensors.append(getFirstCellOccupe(0, -1));
        break;
    case Game::edBas:
        sensors.append(getFirstCellOccupe(0, 1));
        sensors.append(getFirstCellOccupe(-1, 0));
        sensors.append(getFirstCellOccupe(0, -1));
        sensors.append(getFirstCellOccupe(1, 0));
        break;
    case Game::edGauche:
        sensors.append(getFirstCellOccupe(-1, 0));
        sensors.append(getFirstCellOccupe(0, -1));
        sensors.append(getFirstCellOccupe(1, 0));
        sensors.append(getFirstCellOccupe(0, 1));
        break;
    }

    sensors.append(Sensor(pasteque, Sensor::estPasteque));
}

Sensor Game::getFirstCellOccupe(int incX, int incY) const {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;

    // en haut à gauche
    p = QPoint(tete.x() + incX, tete.y() +incY);
    while((type = cellFree(p, true)) == Sensor::estNone) {
       p = QPoint(p.x() + incX, p.y() +incY);
    }

    return Sensor(p, type);
}

void Game::newPasteque() {
    do {
        idPasteque = (idPasteque + 1) % pasteques.size();
        pasteque =pasteques[idPasteque];
    }while (cellFree(pasteque, true) != Sensor::estNone);
}

void Game::next() {
    QList<float> entrees;
    QList<float> sorties = reseau.getSorties();
    QPoint tete = snake[0];
    float max = -1;
    int incX, incY;

    for(int i=0;i<sensors.size();i++) {
        if(sensors[i].getType() != Sensor::estPasteque) {
            int diffX = sensors[i].getPoint().x() - tete.x();
            int diffY = sensors[i].getPoint().y() - tete.y();

            entrees.append(diffX / (float)(largeur - 2));
            entrees.append(diffY / (float)(hauteur - 2));
        }
    }

    getIncs(incX, incY);

    entrees.append((pasteque.x() - tete.x()) / (float)(largeur - 2));
    entrees.append((pasteque.y() - tete.y()) / (float)(hauteur - 2));
    entrees.append(incX);
    entrees.append(incY);

    sorties = reseau.eval(entrees);

    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] > max && sorties[i] > 0.5) {
            if(i == 1) {
                if(incX != 0) {
                    setDirection(0, (i == 1 ? -1 : 1) * incX);
                } else {
                    setDirection((i == 1 ? 1 : -1) * incY, 0);
                }
            }

            lastSortie = i;
            max = sorties[i];
        }
    }
}

void Game::initReseau() {
    reseau.addCouche(Couche(12, 24));
    reseau.addCouche(Couche(24, 3));
}

void Game::getIncs(int &incX, int &incY) const {
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

void Game::setDirection(int incX, int incY) {
    if(incX == 0 && incY == -1) {
        direction = Game::edHaut;
    } else if(incX == 1 && incY == 0) {
        direction = Game::edDroite;
    } else if(incX == 0 && incY == 1) {
        direction = Game::edBas;
    } else {
        direction = Game::edGauche;
    }
}



