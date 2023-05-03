#ifndef NEURONE_H
#define NEURONE_H

#include <QList>

#define MAX_POIDS ((float)20.0)

class Neurone {
public:
    Neurone(int nbEntree);
    Neurone(const QList<float>& poids);
    void eval(const QList<float>& entree);
    QList<float> getPoids() const;
    float getValue() const;

    static float generePoid();
private:
    int nbEntree;
    QList<float> poids;
    float value;
};

#endif //NEURONE_H
