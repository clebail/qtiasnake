#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#define MAX_POIDS   ((float)200.0)

class Random
{
public:
    static int maxPoids;
    Random();

    static float generePoid();
    static void reset();
};

#endif // RANDOM_H
