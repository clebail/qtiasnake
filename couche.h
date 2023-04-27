#ifndef COUCHE_H
#define COUCHE_H

#include "neurone.h"

class Couche {
public:
    Couche(int nbEntree, int nbSortie);
    Couche(QList<QList<float> > poids);
    QList<float> eval(const QList<float>& entrees);
    QList<Neurone> getNeurones() const;
    int getNbEntree() const;
    int getNbSortie() const;
    QList<float> getSorties() const;
private:
    int nbEntree;
    QList<Neurone> neurones;
};

#endif //COUCHE_H
