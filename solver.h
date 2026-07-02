#ifndef SOLVER_H
#define SOLVER_H

#include "game.h"

class Solver {
public:
    Solver();
    Solver(Game *game);

    Game::Direction next() const;
private:
    Game *game;
    QVector<int> cycles;
    int gLargeur;
    int gHauteur;
    int gSize;

    Game::Direction hamiltonianDir(int x, int y, Game::Direction direction) const;
};

#endif // SOLVER_H
