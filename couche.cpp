#include <QtDebug>
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

QList<float> Couche::eval(const QList<float>& entrees, bool last) {
    QList<float> result;
    float max = 0;
    int active = 0;
    bool first = true;

    for(int i=0;i<neurones.size();i++) {
        float sumEntrees;

        neurones[i].eval(entrees, sumEntrees);

        if(neurones[i].getValue() >= SEUIL && (first || sumEntrees > max)) {
            max = sumEntrees;
            active = i;
            first = false;
        }
    }

    for(int i=0;i<neurones.size();i++) {
        if(last)
        {
            if(i == active) {
                neurones[i].active();
            } else {
                neurones[i].reset();
            }
        }

        result.append(neurones[i].getValue());
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

QList<float> Couche::getSorties() const {
    QList<float> values;

    for(int i=0;i<neurones.size();i++) {
        values.append(neurones[i].getValue());
    }

    return values;
}

QList<QList<float> > Couche::getPoids() const {
    QList<QList<float> > poids;

    for(int i=0;i<neurones.size();i++) {
        poids.append(neurones[i].getPoids());
    }

    return poids;
}
