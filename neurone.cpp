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

    // this->poids.append(Neurone::generePoid());
}

void Neurone::eval(const QList<float>& entree) {
    float sum = 0;
    int i;
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

float Neurone::generePoid() {
    return ((rand() % 1000) - 500) * MAX_POIDS / 500.0;
}
