#include <QList>

#define MAX_POIDS ((float)100)

class Neurone {
public:
    Neurone(int nbEntree);
    Neurone(const QList<float>& poids);
    float eval(const QList<float>& entree) const;
    QList<float> getPoids() const;
private:
    int nbEntree;
    QList<float> poids;
};
