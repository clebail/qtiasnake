#include "game.h"

Game::Game(int largeur, int hauteur) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    snake.append(QPoint(5, 5));
    snake.append(QPoint(5, 6));
    snake.append(QPoint(6, 6));
    snake.append(QPoint(6, 7));

    pasteque = QPoint(7, 2);
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

const QPoint&Game:: getPasteque() const {
    return pasteque;
}

