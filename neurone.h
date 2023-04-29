#ifndef NEURONE_H
#define NEURONE_H

#include <QList>

#define MAX_POIDS ((float)5.0)

class Neurone {
public:
    Neurone(int nbEntree);
    Neurone(const QList<float>& poids);
    void eval(const QList<float>& entree);
    QList<float> getPoids() const;
    float getValue() const;
    QList<float> fusion(const Neurone& other) const;
private:
    int nbEntree;
    QList<float> poids;
    float value;

    float generePoid() const;
};

#endif //NEURONE_H
