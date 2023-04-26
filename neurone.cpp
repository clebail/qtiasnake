#include <math.h>
#include "neurone.h"

Neurone::Neurone(int nbEntree) {
    this->nbEntree = nbEntree;
    for(int i=0;i<nbEntree;i++) {
        poids.append((rand() % (int)(MAX_POIDS * 2 + 1)) - MAX_POIDS);
    }
}

Neurone::Neurone(const QList<float>& poids) {
    this->nbEntree = poids.size();
    this->poids = QList<float>(poids);
}

float Neurone::eval(const QList<float>& entree) const {
    float sum = 0;
    for(int i=0;i<nbEntree;i++) {
        sum += entree.at(i) * poids.at(i);
    }
    
    return 1.0 / (1.0 + exp(-sum));
}

QList<float> Neurone::getPoids() const {
    return poids;
}
