#include <QtDebug>
#include "game.h"

Game::Game(int largeur, int hauteur) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    snake.append(QPoint(5, 5));
    nbMouvement = totMouvement = 0;

    incY = -1;
    incX = 0;

    newPasteque();

    calculSensors();
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
    if(snake[0].y() == 1 && incY == -1) {
        incX = 1;
        incY = 0;
    }

    if(snake[0].x() == largeur - 2 && incX == 1) {
        incX = 0;
        incY = 1;
    }

    if(snake[0].y() == hauteur - 2 && incY == 1) {
        incX = -1;
        incY = 0;
    }

    if(snake[0].x() == 1 && incX == -1) {
        incX = 0;
        incY = -1;
    }
}
