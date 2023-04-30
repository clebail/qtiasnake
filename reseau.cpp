#include "reseau.h"

Reseau::Reseau() {
}

Reseau::Reseau(const Couche& couche) {
    couches.append(couche);
}

Reseau::Reseau(const QList<Couche>& couches) {
    this->couches = couches;
}

Reseau::Reseau(const Reseau::Poids &poids) {
    for(int i=0;i<poids.size();i++) {
        couches.append(Couche(poids[i]));
    }
}


bool Reseau::addCouche(const Couche& couche) {
    if(couches.size()) {
        Couche last = couches.last();

        if(last.getNbSortie() == couche.getNbEntree()) {
            couches.append(couche);

            return true;
        }

        return false;
    }

    couches.append(couche);
    return true;
}

QList<float> Reseau::eval(const QList<float>& entrees) {
    QList<float> es = entrees;

    for(int i=0;i<couches.size();i++) {
        es = couches[i].eval(es);
    }
    
    return es;
}

QList<Couche> Reseau::getCouches() const {
    return couches;
}

QList<float> Reseau::getSorties() const {
    Couche couche=couches.last();

    return couche.getSorties();
}

Reseau::Poids Reseau::getPoids() const {
    Reseau::Poids poids;

    for(int i=0;i<couches.size();i++) {
        poids.append(couches[i].getPoids());
    }

    return poids;
}
