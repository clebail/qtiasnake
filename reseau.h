#include "couche.h"

class Reseau {
public:
    Reseau();
    Reseau(const Couche& couche);
    Reseau(const QList<Couche>& couches);
    bool addCouche(const Couche& couche);
    QList<float> eval(const QList<float>& entrees) const;
    QList<Couche> getCouches() const;
private:
    QList<Couche> couches;
};
