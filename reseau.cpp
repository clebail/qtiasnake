#include "reseau.h"

Reseau::Reseau() {
}

Reseau::Reseau(const Couche& couche) {
    couches.append(couche);
}

Reseau::Reseau(const QList<Couche>& couches) {
    this->couches = couches;
}

bool Reseau::addCouche(const Couche& couche) {
    Couche last = couches.last();
    
    if(last.getNbSortie() == couche.getNbEntree()) {
        couches.append(couche);
        
        return true;
    }
    
    return false;
}

QList<float> Reseau::eval(const QList<float>& entrees) const {
    QList<float> es = entrees;
    QList<Couche>::const_iterator i;
    
    for(i=couches.begin();i!=couches.end();++i) {
        es = (*i).eval(es);
    }
    
    return es;
}

QList<Couche> Reseau::getCouches() const {
    return couches;
}
