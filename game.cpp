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

    incY = -1;
    incX = 0;

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

int Game::getIncX() const {
    return incX;
}

int Game::getIncY() const {
    return incY;
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
    gr.score = (snake.size() - 1) * 10 - (perdu ? 10 : 0);

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

    sensors.append(getFirstCellOccupe(0, -1));
    sensors.append(getFirstCellOccupe(0, 1));
    sensors.append(getFirstCellOccupe(-1, 0));
    sensors.append(getFirstCellOccupe(1, 0));

    sensors.append(Sensor(pasteque, Sensor::estPasteque));
}

Sensor Game::getFirstCellOccupe(int incX, int incY) const {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;

    // en haut à gauche
    p = QPoint(tete.x() + incX, tete.y() +incY);
    while((type = cellFree(p)) == Sensor::estNone) {
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
    int diffX, diffY;
    float angle;
    float max = -1;

    entrees.append(cellFree(QPoint(tete.x(), tete.y() - 1), true) == Sensor::estNone ? 1 : 0);
    entrees.append(cellFree(QPoint(tete.x() + 1, tete.y()), true) == Sensor::estNone ? 1 : 0);
    entrees.append(cellFree(QPoint(tete.x(), tete.y() + 1), true) == Sensor::estNone ? 1 : 0);
    entrees.append(cellFree(QPoint(tete.x() - 1, tete.y()), true) == Sensor::estNone ? 1 : 0);


    diffX = pasteque.x() - tete.x();
    diffY = pasteque.y() - tete.y();

    if(diffY == 0) {
        angle = diffX > 0 ? 0 : PI;
    } else if(diffX == 0) {
        angle = diffY > 0 ? PI/2 : 3*PI/2;
    } else {
        if(abs(diffY) > abs(diffX)) {
            angle = acos((float)diffX / (float)abs(diffY));
        } else {
            angle = asin((float)diffY / (float)abs(diffX));
        }
    }

    entrees.append(angle / (2 * PI));
    entrees.append(sqrt(diffX * diffX + diffY - diffY) / diagonale);
    entrees.append(lastSortie == 0 ? 1.0 : 0.0);
    entrees.append(lastSortie == 1 ? 1.0 : 0.0);
    entrees.append(lastSortie == 2 ? 1.0 : 0.0);

    sorties = reseau.eval(entrees);

    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] > max) {
            if(i >= 1) {
                if(incX != 0) {
                    incY = (i == 1 ? -1 : 1) * incX;
                    incX = 0;
                } else {
                    incX = (i == 1 ? 1 : -1) * incY;
                    incY = 0;
                }
            }

            lastSortie = i;
            max = sorties[i];
        }
    }
}

void Game::initReseau() {
    reseau.addCouche(Couche(9, 6));
    reseau.addCouche(Couche(6, 3));
}


