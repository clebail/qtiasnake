#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#define MAX_POIDS   ((float)1.0)
#define MUTE_STEP   ((float)0.1)
#define RESET_RATE  5                   // % de poids réinitialisés
#define MUTE_RATE_START   40            // % de poids perturbés (génération 0)
#define MUTE_RATE_END     8             // % de poids perturbés (après décroissance)
#define MUTE_SIGMA_START  ((float)0.3)  // écart-type du bruit (génération 0)
#define MUTE_SIGMA_END    ((float)0.1)  // écart-type du bruit (après décroissance)
#define MUTE_DECAY_GEN    50            // nb de générations de décroissance

class Random
{
public:
    static int maxPoids;
    Random();

    static float generePoid();
    static float muteNoise(float sigma);
    static void reset();
};

#endif // RANDOM_H
