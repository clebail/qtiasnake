#ifndef NEURONE_H
#define NEURONE_H

#include <QList>

#define SEUIL       ((float)0.8)

class Neurone {
public:
    typedef enum { efSigmoide, efRelu, efTanh } Function;

    Neurone(int nbEntreeconst, const Function& function = efSigmoide);
    Neurone(const QList<float>& poids, const Function& function = efSigmoide);
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
    Function function;

    float sigmoide(float) const;
    float relu(float) const;
    float tanh(float) const;
};

#endif //NEURONE_H
