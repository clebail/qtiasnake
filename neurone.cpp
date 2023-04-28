#include <QtDebug>
#include <math.h>
#include "neurone.h"

Neurone::Neurone(int nbEntree) {
    this->nbEntree = nbEntree;
    for(int i=0;i<nbEntree;i++) {
        float f = ((rand() % 1000) - 500) * MAX_POIDS / 500.0;
        poids.append(f);
    }
}

Neurone::Neurone(const QList<float>& poids) {
    this->nbEntree = poids.size();
    this->poids = QList<float>(poids);
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
