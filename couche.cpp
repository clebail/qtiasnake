#include "couche.h"

Couche::Couche(int nbEntree, int nbSortie) {
    this->nbEntree = nbEntree;
    
    for(int i=0;i<nbSortie;i++) {
        neurones.append(Neurone(nbEntree));
    }
}

Couche::Couche(QList<QList<float> > poids) {
    QList<QList<float> >::iterator i;
    
    for(i=poids.begin();i!=poids.end();++i) {
        nbEntree = (*i).size();
        neurones.append(Neurone(*i));
    }
}

QList<float> Couche::eval(const QList<float>& entrees) const {
    QList<float> result;
    QList<Neurone>::const_iterator i;
    
    for(i=neurones.begin();i!=neurones.end();++i) {
        result.append((*i).eval(entrees));
    }
    
    return result;
}

QList<Neurone> Couche::getNeurones() const {
    return neurones;
}

int Couche::getNbSortie() const {
    return neurones.size();
}

int Couche::getNbEntree() const {
    return nbEntree;
}
