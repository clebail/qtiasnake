#include "random.h"

static std::default_random_engine generator;
static std::normal_distribution<float> distribution(0.0, MAX_POIDS);

Random::Random()
{

}

float Random::generePoid() {
    // return distribution(generator);
    return (rand() % 1000 - 500) * MAX_POIDS / 500.0;
}

void Random::reset() {
    // distribution.reset();
}
