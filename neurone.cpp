#include <QtDebug>
#include <math.h>
#include "neurone.h"

Neurone::Neurone(int nbEntree) {
    this->nbEntree = nbEntree;
    for(int i=0;i<nbEntree;i++) {
        poids.append(generePoid());
    }
}

Neurone::Neurone(const QList<float>& poids) {
    this->nbEntree = poids.size();
    this->poids = poids;
}

void Neurone::eval(const QList<float>& entree) {
    float sum = 0;
    for(int i=0;i<nbEntree;i++) {
        sum += entree.at(i) * poids.at(i);
    }
    
    value = 1.0 / (1.0 + exp(-sum));
}

QList<float> Neurone::getPoids() const {
    return poids;
}

float Neurone::getValue() const {
    return value;
}

QList<float> Neurone::fusion(const Neurone& other) const {
    QList<float> result;

    for(int i=0;i<poids.size();i++) {
        switch(rand() % 3) {
        case 0:
            result.append(poids[i]);
            break;
        case 1:
            result.append(other.poids[i]);
            break;
        case 2:
            result.append(generePoid());
            break;
        }
    }

    return result;
}

float Neurone::generePoid() const {
    return ((rand() % 1000) - 500) * MAX_POIDS / 500.0;
}
