#include "solver.h"

#define MARGE           3

typedef struct _Direction {
    int dx;
    int dy;

    _Direction(int dx, int dy) {
        this->dx = dx;
        this->dy = dy;
    }
}Direction;

static Direction directions[NB_DIRECTION] = { Direction(0, -1), Direction(1, 0), Direction(0, 1), Direction(-1, 0) };

Solver::Solver() {
}

Solver::Solver(Game *game) {
    QPoint head = game->getSnake().first();
    int x = head.x();
    int y = head.y();
    Game::Direction dir = game->getDirection();

    this->game = game;
    gLargeur = game->getLargeur() - 2;
    gHauteur = game->getHauteur() - 2;
    gSize = gLargeur * gHauteur;
    cycles.resize(gSize);

    for (int i=0;i<gSize;i++) {
        int idx = (x - 1) + (y - 1) * gLargeur;

        dir = hamiltonianDir(x, y, dir);
        cycles[idx] = i;
        x += directions[dir].dx;
        y += directions[dir].dy;
    }
}

Game::Direction Solver::next() const {
    const QPoint& head = game->getSnake().at(0);
    int idxHead = (head.x() - 1) + (head.y() - 1) * gLargeur;
    int idxPasteque = (game->getPasteque().x() - 1) + (game->getPasteque().y() - 1) * gLargeur;
    int idxQueue = (game->getSnake().last().x() - 1) + (game->getSnake().last().y() - 1) * gLargeur;
    int distPasteque = ((cycles[idxPasteque] - cycles[idxHead]) % gSize + gSize) % gSize;
    int distQueue = ((cycles[idxQueue] - cycles[idxHead]) % gSize + gSize) % gSize;
    int coupeMax = qMin(distQueue - MARGE, distPasteque);
    int maxVoisin = 0;
    Game::Direction dir = hamiltonianDir(head.x(), head.y(), game->getDirection());

    for(int i=0;i<NB_DIRECTION;i++) {
        int x = head.x() + directions[i].dx;
        int y = head.y() + directions[i].dy;
        QPoint voisin(x, y);

        if(game->isLibre(voisin) || voisin == game->getSnake().last()) {
            int idx = (x - 1) + (y - 1) * gLargeur;
            int dist = ((cycles[idx] - cycles[idxHead]) % gSize + gSize) % gSize;

            if(dist >= 1 && dist <= coupeMax && dist > maxVoisin) {
                maxVoisin = dist;
                dir = (Game::Direction)i;
            }
        }
    }

    return dir;
}

Game::Direction Solver::hamiltonianDir(int x, int y, Game::Direction direction) const {
    bool pair = x % 2 == 0;

    if(x == game->getLargeur() - 2 && y == game->getHauteur() - 3) {
        return Game::edBas;
    }

    if(y >= game->getHauteur() - 2 && x > 1) {
        return Game::edGauche;
    }

    if(y == game->getHauteur() - 3) {
        if(pair && direction == Game::edBas) {
            return Game::edDroite;
        }
    }

    if(y == 1) {
        if(!pair && direction == Game::edHaut) {
            return Game::edDroite;
        }
    }

    return !pair ? Game::edHaut : Game::edBas;
}
