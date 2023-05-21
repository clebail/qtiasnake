#ifndef RESEAU_H
#define RESEAU_H

#include "couche.h"

class Reseau {
public:
    typedef QList<QList<QList<float> > > Poids;

    Reseau();
    Reseau(const Couche& couche);
    Reseau(const QList<Couche>& couches);
    Reseau(const Reseau::Poids &poids, const QList<Neurone::Function>& functions);
    bool addCouche(const Couche& couche);
    QList<float> eval(const QList<float>& entrees);
    QList<Couche> getCouches() const;
    QList<float> getSorties() const;
    Reseau::Poids getPoids() const;
    QList<float> getEntress() const;
    int getNbNeurone() const;
private:
    QList<Couche> couches;
    QList<float> entrees;
};

#endif //RESEAU_H
