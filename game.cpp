#include "game.h"

Game::Game(int largeur, int hauteur) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    snake.append(QPoint(5, 5));

    pasteque = QPoint(7, 3);

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

const QList<QPoint>&  Game::getSensors() const {
    return sensors;
}

const QPoint& Game::getPasteque() const {
    return pasteque;
}

void Game::step(int incX, int incY) {
    snake[0].setX(snake[0].x() + incX);
    snake[0].setY(snake[0].y() + incY);

    calculSensors();
}

bool Game::cellFree(const QPoint& p) const {
    int x = p.x();
    int y= p.y();

    if(x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1) {
        QList<QPoint>::const_iterator i;

        for(i=snake.begin();i!=snake.end();++i) {
            if((*i).x() == x && (*i).y() == y) {
                return false;
            }
        }

        return pasteque.x() != x || pasteque.y() != y;
    }

    return false;
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

QPoint Game::getFirstCellOccupe(int incX, int incY) const {
    QPoint tete = snake.at(0);
    QPoint p;

    // en haut à gauche
    p = QPoint(tete.x() + incX, tete.y() +incY);
    while(cellFree(p)) {
       p = QPoint(p.x() + incX, p.y() +incY);
    }

    return p;
}

