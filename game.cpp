#include <QtDebug>
#include <math.h>
#include "game.h"

Game::Game(int largeur, int hauteur, const Reseau::Poids &) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    diagonale = sqrt(largeur * largeur + hauteur * hauteur);

    snake.append(QPoint(largeur/2, hauteur/2));
    nbMouvement = totMouvement = 0;

    incY = -1;
    incX = 0;

    newPasteque();

    calculSensors();

    initReseau();
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

    newTete.setX(snake[0].x() + incX);
    newTete.setY(snake[0].y() + incY);

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

        if(nbMouvement == 100) {
            return false;
        }

        calculSensors();
        next();

        return true;

    }

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
    gr.score = pow(totMouvement, snake.size());

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

    sensors.append(getFirstCellOccupe(-1, -1));
    sensors.append(getFirstCellOccupe(0, -1));
    sensors.append(getFirstCellOccupe(1, -1));
    sensors.append(getFirstCellOccupe(-1, 0));
    sensors.append(getFirstCellOccupe(1, 0));
    sensors.append(getFirstCellOccupe(-1, 1));
    sensors.append(getFirstCellOccupe(0, 1));
    sensors.append(getFirstCellOccupe(1, 1));
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
        pasteque = QPoint(rand() % (largeur - 2) + 1, rand() % (hauteur - 2) + 1);
    }while (cellFree(pasteque, true) != Sensor::estNone);
}

void Game::next() {
    QList<float> entrees;
    QList<float> sorties = reseau.getSorties();
    QPoint tete = snake[0];
    float max = 0.0;

    for(int i=0;i<sensors.size();i++) {
        Sensor sensor = sensors[i];
        float dist;
        if(tete.x() != sensor.getPoint().x()) {
            if(tete.y() !=  sensor.getPoint().y()) {
                float x = tete.x() - sensor.getPoint().x();
                float y = tete.y() - sensor.getPoint().y();
                dist = sqrt(x * x + y * y) / diagonale;
            } else {
                dist = abs(tete.x() - sensor.getPoint().x()) / (float)largeur;
            }
        } else {
            dist = abs(tete.y() - sensor.getPoint().y()) / (float)hauteur;
        }

        entrees.append(dist / diagonale);
        entrees.append(sensor.getType() == Sensor::estPasteque ? 1.0 : 0.0);
    }
    entrees.append(sorties[0]);
    entrees.append(sorties[1]);
    entrees.append(sorties[2]);
    entrees.append(sorties[3]);

    sorties = reseau.eval(entrees);

    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] >= 0.5 && sorties[i] > max) {
            switch(i) {
                case 0:
                    incX = 0;
                    incY = -1;
                break;
                case 1:
                    incX = 1;
                    incY = 0;
                break;
                case 2:
                    incX = 0;
                    incY = 1;
                break;
                case 3:
                    incX = -1;
                    incY = 0;
                break;
            }

            max =  sorties[i];
        }
    }
}

void Game::initReseau() {
    reseau.addCouche(Couche(20,16));
    reseau.addCouche(Couche(16,12));
    reseau.addCouche(Couche(12,8));
    reseau.addCouche(Couche(8,4));
}


