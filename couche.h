#include "neurone.h"

class Couche {
public:
    Couche(int nbEntree, int nbSortie);
    Couche(QList<QList<float> > poids);
    QList<float> eval(const QList<float>& entrees) const;
    QList<Neurone> getNeurones() const;
    int getNbEntree() const;
    int getNbSortie() const;
private:
    int nbEntree;
    QList<Neurone> neurones;
};
