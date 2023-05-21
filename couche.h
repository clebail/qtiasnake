#ifndef COUCHE_H
#define COUCHE_H

#include "neurone.h"

class Couche {
public:
    Couche(int nbEntree, int nbSortie, const Neurone::Function& function = Neurone::efSigmoide);
    Couche(QList<QList<float> > poids, const Neurone::Function& function = Neurone::efSigmoide);
    QList<float> eval(const QList<float>& entrees, bool last = false);
    QList<Neurone> getNeurones() const;
    int getNbEntree() const;
    int getNbSortie() const;
    QList<float> getSorties() const;
    QList<QList<float> > getPoids() const;
    int getNbNeurone() const;
private:
    int nbEntree;
    QList<Neurone> neurones;
};

#endif //COUCHE_H
