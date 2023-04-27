#ifndef RESEAU_H
#define RESEAU_H

#include "couche.h"

class Reseau {
public:
    Reseau();
    Reseau(const Couche& couche);
    Reseau(const QList<Couche>& couches);
    bool addCouche(const Couche& couche);
    QList<float> eval(const QList<float>& entrees);
    QList<Couche> getCouches() const;
    QList<float> getSorties() const;
private:
    QList<Couche> couches;
};

#endif //RESEAU_H
