#ifndef RESEAU_H
#define RESEAU_H

#include "couche.h"

class Reseau {
public:
    typedef QList<QList<QList<float> > > Poids;

    Reseau();
    Reseau(const Couche& couche);
    Reseau(const QList<Couche>& couches);
    Reseau(const Reseau::Poids &poids);
    bool addCouche(const Couche& couche);
    QList<float> eval(const QList<float>& entrees);
    QList<Couche> getCouches() const;
    QList<float> getSorties() const;
    Reseau::Poids getPoids() const;
    Reseau::Poids fusion(const Reseau &other) const;
private:
    QList<Couche> couches;
};

#endif //RESEAU_H
