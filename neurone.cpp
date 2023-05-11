#include <QtDebug>
#include <math.h>
#include "neurone.h"

Neurone::Neurone(int nbEntree) {
    this->nbEntree = nbEntree;
    for(int i=0;i<nbEntree;i++) {
        poids.append(Neurone::generePoid());
    }

    poids.append(Neurone::generePoid());
}

Neurone::Neurone(const QList<float>& poids) {
    this->nbEntree = poids.size() - 1;
    this->poids = poids;
}

void Neurone::eval(const QList<float>& entree, float &sum) {
    int i;

    value = 0.0;
    sum = 0.0;
    for(i=0;i<nbEntree;i++) {
        sum += entree.at(i) * poids.at(i);
    }

    sum += poids.at(i);
    
    value = (1.0 / (1.0 + exp(-sum)));
}

QList<float> Neurone::getPoids() const {
    return poids;
}

float Neurone::getValue() const {
    return value;
}

void Neurone::active() {
    value = 1.0;
}

void Neurone::reset() {
    value = 0.0;
}

float Neurone::generePoid() {
    return ((rand() % 1000) - 500) * MAX_POIDS / 500.0;
}
