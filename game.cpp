#include <cstdlib>
#include "game.h"

Game::Game(int largeur, int hauteur) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    snake.append(QPoint(largeur / 2, hauteur / 2));
    snake.append(QPoint(largeur / 2, hauteur / 2 + 1));
    snake.append(QPoint(largeur / 2, hauteur / 2 + 2));
    snake.append(QPoint(largeur / 2, hauteur / 2 + 3));

    occupe = QVector<char>(largeur * hauteur, 0);
    foreach(const QPoint& p, snake) {
        occupe[idx(p)] = 1;
    }

    direction = Game::edHaut;
    gagne = false;

    nextPasteque();
}

int Game::getLargeur() const {
    return largeur;
}

int Game::getHauteur() const {
    return hauteur;
}

const QList<QPoint>& Game::getSnake() const {
    return snake;
}

const QPoint& Game::getPasteque() const {
    return pasteque;
}

Game::Direction Game::getDirection() const {
    return direction;
}

void Game::setDirection(const Game::Direction& direction) {
    this->direction = direction;
}

int Game::idx(const QPoint& p) const {
    return p.x() + p.y() * largeur;
}

bool Game::isOccupe(const QPoint& p) const {
    if(p.x() < 0 || p.x() >= largeur || p.y() < 0 || p.y() >= hauteur) {
        return false;
    }
    return occupe[idx(p)] != 0;
}

bool Game::isLibre(const QPoint& p) const {
    if(p.x() <= 0 || p.x() >= largeur - 1 || p.y() <= 0 || p.y() >= hauteur - 1) {
        return false;   // mur ou hors plateau
    }
    return occupe[idx(p)] == 0;
}

bool Game::estGagne() const {
    return gagne;
}

void Game::getIncs(const Game::Direction& direction, int& incX, int& incY) const {
    switch(direction) {
    case Game::edHaut:   incX = 0;  incY = -1; return;
    case Game::edDroite: incX = 1;  incY = 0;  return;
    case Game::edBas:    incX = 0;  incY = 1;  return;
    case Game::edGauche: incX = -1; incY = 0;  return;
    }
}

bool Game::step() {
    if(gagne) {
        return false;
    }

    int incX, incY;
    getIncs(direction, incX, incY);

    QPoint newTete(snake[0].x() + incX, snake[0].y() + incY);
    QPoint last = snake.last();

    bool mur = newTete.x() <= 0 || newTete.x() >= largeur - 1 ||
               newTete.y() <= 0 || newTete.y() >= hauteur - 1;
    bool mangePasteque = (newTete == pasteque);
    // Collision corps, sauf la queue qui va libérer sa case (si on ne grandit pas).
    bool corps = isOccupe(newTete) && !(newTete == last && !mangePasteque);

    if(mur || corps) {
        return false;   // mort
    }

    // Avance le corps.
    for(int i = snake.size() - 1; i >= 1; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newTete;

    if(mangePasteque) {
        snake.append(last);           // la queue reste : le serpent grandit
        occupe[idx(newTete)] = 1;

        if(snake.size() >= (largeur - 2) * (hauteur - 2)) {
            gagne = true;
            return false;             // plateau rempli
        }

        nextPasteque();
    } else {
        occupe[idx(last)] = 0;        // la queue libère sa case
        occupe[idx(newTete)] = 1;     // (dans cet ordre : gère tête == ancienne queue)
    }

    return true;
}

QPoint Game::newPasteque() const {
    QPoint p;
    do {
        p = QPoint(rand() % (largeur - 2) + 1, rand() % (hauteur - 2) + 1);
    } while(occupe[idx(p)] != 0);

    return p;
}

void Game::nextPasteque() {
    pasteque = newPasteque();
}
