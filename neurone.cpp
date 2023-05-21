#include <QtDebug>
#include <math.h>
#include "neurone.h"
#include "random.h"

Neurone::Neurone(int nbEntree, const Neurone::Function& function) {
    this->nbEntree = nbEntree;
    this->function = function;

    for(int i=0;i<nbEntree;i++) {
        poids.append(Random::generePoid());
    }

    poids.append(Random::generePoid());
}

Neurone::Neurone(const QList<float>& poids, const Neurone::Function& function) {
    this->nbEntree = poids.size() - 1;
    this->function = function;
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
    
    switch(function) {
    case Neurone::efSigmoide:
         value = sigmoide(sum);
        break;
    case Neurone::efRelu:
        value = relu(sum);
        break;
    default:
        value = tanh(sum);
    }


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

float Neurone::sigmoide(float v) const {
    return (1.0 / (1.0 + exp(-v)));
}

float Neurone::relu(float v) const {
    return qMax(0.0F, v);
}

float Neurone::tanh(float v) const {
    return tanhf(v);
}
