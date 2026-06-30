#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#define MAX_POIDS   ((float)1.0)
#define MUTE_STEP   ((float)0.1)
#define MUTE_RATE   8               // % de poids perturbés (bruit gaussien)
#define RESET_RATE  5               // % de poids réinitialisés
#define MUTE_SIGMA  ((float)0.1)    // écart-type du bruit de mutation

class Random
{
public:
    static int maxPoids;
    Random();

    static float generePoid();
    static float muteNoise();
    static void reset();
};

#endif // RANDOM_H
