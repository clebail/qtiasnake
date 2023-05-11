#ifndef NEURONE_H
#define NEURONE_H

#include <QList>

#define MAX_POIDS   ((float)100)
#define SEUIL       ((float)0.8)

class Neurone {
public:
    Neurone(int nbEntree);
    Neurone(const QList<float>& poids);
    void eval(const QList<float>& entree, float& sum);
    QList<float> getPoids() const;
    float getValue() const;
    void active();
    void reset();

    static float generePoid();
private:
    int nbEntree;
    QList<float> poids;
    float value;
};

#endif //NEURONE_H
