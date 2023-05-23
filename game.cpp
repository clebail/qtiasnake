#include <QtDebug>
#include <math.h>
#include "game.h"

#define NB_SORTIE           4
#define PI                  ((float)(3.14159))

static QList<QPoint> pasteques;
static QList<Neurone::Function> functions;

QDebug operator<<(QDebug debug, const Sensor &sensor)
{
    QDebugStateSaver saver(debug);

    debug.nospace().noquote() << sensor.getType() << ": " << sensor.getPoint();

    return debug;
}

Game::Game(int largeur, int hauteur, const Reseau::Poids &poids) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    diagonale = sqrt((largeur - 1) * (largeur - 1) + (hauteur - 1) * (hauteur - 1));

    maxMouvement = (largeur - 2) * (hauteur - 2);

    snake.append(QPoint(largeur/2, hauteur/2));
    snake.append(QPoint(largeur/2, hauteur/2 + 1));
    snake.append(QPoint(largeur/2, hauteur/2 + 2));
    snake.append(QPoint(largeur/2, hauteur/2 + 3));
    nbMouvement = totMouvement = 0;

    direction = Game::edHaut;
    queueDirection = Game::edHaut;
    nbCgtDir = 0;

    idPasteque = -1;
    nextPasteque();

    calculSensors();

    if(poids.size() == 0) {
        initReseau();
    } else {
        reseau = Reseau(poids, functions);
    }

    caseVisite.clear();
    caseVisite.insert(snake[0].x() + snake[0].y() * largeur, 1);

    perdu = turnRight = turnLeft = false;
    turns[0] = turns[1] = 0;
    limiteVisite = LIMITE_VISITE;
}

int Game::getLargeur() {
    return largeur;
}

int Game::getHauteur() {
    return hauteur;
}

const QList<QPoint>& Game::getSnake() const {
    return snake;
}

const QList<Sensor>&  Game::getSensors() const {
    return sensors;
}

const QPoint& Game::getPasteque() const {
    return pasteque;
}

bool Game::step() {
    int i;
    QPoint last = snake.last();
    QPoint newTete;
    Sensor::ESensorType type;
    int idCase;
    int incX, incY;

    getIncs(direction, incX, incY);
    newTete.setX(snake[0].x() + incX);
    newTete.setY(snake[0].y() + incY);

    idCase = newTete.x() + newTete.y() * largeur;

    type = cellFree(newTete);
    if(type != Sensor::estMur && type != Sensor::estSnake)
    {
        bool first = true;
        for(i=snake.size()-1;i>=1;i--) {
            if(first) {
                incX = snake[i-1].x() - snake[i].x();
                incY = snake[i-1].y() - snake[i].y();
                queueDirection = calculDirection(incX, incY);
            }
            snake[i].setX(snake[i-1].x());
            snake[i].setY(snake[i-1].y());
            first = false;
        }

        snake[0] = newTete;
        nbMouvement++;       

        if(type == Sensor::estPasteque) {
            snake.append(last);

            nextPasteque();
            nbMouvement = 0;
            limiteVisite += LIMITE_VISITE;
        }

        if(!caseVisite.contains(idCase)) {
            caseVisite.insert(idCase, 1);
        } else {
            caseVisite[idCase]++;
            if(caseVisite[idCase] == limiteVisite) {
                return false;
            }
        }

        totMouvement++;
        /*if(nbMouvement == maxMouvement) {

            perdu = snake.size() == 4 && caseVisite.size() < 50;
            perdu |= (!turnLeft || !turnRight);

            return false;
        }*/

        calculSensors();
        next();

        return true;

    }

    perdu = true;

    return false;
}

int Game::getTotMouvement() const {
    return totMouvement;
}

int Game::getNbMouvement() const {
    return nbMouvement;
}

int Game::getMaxMouvement() const {
    return maxMouvement;
}

const Reseau& Game::getReseau() const {
    return reseau;
}

void Game::setReseau(const Reseau& reseau) {
    this->reseau = reseau;
}

Game::GameResult Game::getResult() const {
    Game::GameResult gr;
    float m = 0;

    foreach(auto i, caseVisite.values()) {
        m += i;
    }

    m /= (float)caseVisite.size();

    gr.poids = reseau.getPoids();
    //gr.score = (totMouvement * 10 / (nbCgtDir ?: 1)+ (snake.size() - 4) * 10000) * (perdu ? 0 : 1);
    //gr.score = ((snake.size() - 4) * 1000) * (perdu ? 0 : 1);
    //gr.score = (qMax(caseVisite.size() / m, 1.0F) + turns[0] + turns[1] + (snake.size() - 4) * 10000) * (perdu ? 0 : 1);
    gr.scoreVisite = caseVisite.size() * (perdu ? 0 : 1) * (!turns[0] || !turns[1] ? 0 : 1);
    gr.scorePasteque =(snake.size() - 4) * (perdu ? 0 : 1) * (!turns[0] || !turns[1] ? 0 : 1);

    gr.perdu = perdu;

    return gr;
}

int Game::getNbCaseVisite() const {
    return caseVisite.size();
}

int Game::getNbNeurone() const {
    return reseau.getNbNeurone();
}

QMap<int, int> Game::getCaseVisite() const {
    return caseVisite;
}

Sensor::ESensorType Game::cellFree(const QPoint& p, const Sensor::ESensorType &toIgnore) const {
    int x = p.x();
    int y = p.y();

    if(x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1) {
        if(toIgnore != Sensor::estSnake) {
            QList<QPoint>::const_iterator i;

            for(i=snake.begin();i!=snake.end();++i) {
                if((*i).x() == x && (*i).y() == y) {
                    return Sensor::estSnake;
                }
            }
        }

        return toIgnore != Sensor::estPasteque && p == pasteque ? Sensor::estPasteque : Sensor::estNone;
    }

    return Sensor::estMur;
}

void Game::calculSensors() {
    sensors.clear();

    switch(direction) {
    case Game::edHaut:
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        break;
    case Game::edDroite:
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        break;
    case Game::edBas:
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        break;
    case Game::edGauche:
        addSensorsForDirection(0, 1);
        addSensorsForDirection(-1, 1);
        addSensorsForDirection(-1, 0);
        addSensorsForDirection(-1, -1);
        addSensorsForDirection(0, -1);
        addSensorsForDirection(1, -1);
        addSensorsForDirection(1, 0);
        addSensorsForDirection(1, 1);
        break;
    }
}

Sensor Game::getFirstCellOccupe(int incX, int incY, const Sensor::ESensorType& toIgnore) const {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;

    p = QPoint(tete.x() + incX, tete.y() +incY);
    while((type = cellFree(p, toIgnore)) == Sensor::estNone) {
       p = QPoint(p.x() + incX, p.y() +incY);
    }

    return Sensor(p, type);
}

void Game::addSensorsForDirection(int incX, int incY) {
    QPoint tete = snake.at(0);
    QPoint p;
    Sensor::ESensorType type;
    QMap<Sensor::ESensorType, QPoint> tmp;

    p = QPoint(tete.x() + incX, tete.y() + incY);
    while((type = cellFree(p)) != Sensor::estMur) {
        if(type != Sensor::estSnake || !tmp.contains(Sensor::estSnake)) {
            tmp.insert(type, p);
        }
        p = QPoint(p.x() + incX, p.y() +incY);
    }

    sensors.append(Sensor(p, Sensor::estMur));

    if(tmp.contains(Sensor::estSnake)) {
        sensors.append(Sensor(tmp.value(Sensor::estSnake), Sensor::estSnake));
    } else {
        sensors.append(Sensor(tete, Sensor::estSnake));
    }

    if(tmp.contains(Sensor::estPasteque)) {
        sensors.append(Sensor(tmp.value(Sensor::estPasteque), Sensor::estPasteque));
    } else {
        sensors.append(Sensor(tete, Sensor::estPasteque));
    }
}

QPoint Game::newPasteque() const {
    QPoint p;
    do {
        p = QPoint(rand() % (largeur - 2) + 1, rand() % (hauteur - 2) + 1);
    }while (cellFree(p) != Sensor::estNone);

    return p;
}

void Game::nextPasteque() {
    /*if(++idPasteque < pasteques.size()) {
        pasteque = pasteques[idPasteque];
    } else {*/
        pasteque = newPasteque();
        //pasteques.append(pasteque);
    //}
}

void Game::next() {
    QList<float> entrees;
    QList<float> sorties = reseau.getSorties();
    QPoint tete = snake[0];
    /*int incX, incY;
    int queueIncX, queueIncY;*/
    int incS = 0;
    int newDir;
    int goodSortie = 0;

    /*getIncs(direction, incX, incY);
    getIncs(queueDirection, queueIncX, queueIncY);*/

    for(int i=0;i<sensors.size();i++) {
        QPoint p = sensors[i].getPoint();

        if(!p.isNull()) {
            int diffX = abs(p.x() - tete.x());
            int diffY = abs(p.y() - tete.y());

            if(diffX == 0) {
                if(diffY == 0) {
                    entrees.append(0);
                } else {
                    entrees.append(1.0 - (float)diffY / (hauteur - 1));
                }
            } else if(diffY == 0) {
                entrees.append(1.0 - (float)diffX / (largeur - 1));
            } else {
                entrees.append(1.0 - sqrt((float)(diffX * diffX + diffY * diffY)) / diagonale);
            }
        } else {
            entrees.append(0);
        }
    }


    /*entrees.append(incX);
    entrees.append(incY);
    entrees.append(queueIncX);
    entrees.append(queueIncY);*/

    sorties = reseau.eval(entrees);
    for(int i=0;i<sorties.size();i++) {
        if(sorties[i] >= SEUIL) {
            goodSortie = i;
        }
    }

    if(goodSortie == 1) {
        incS = -1;
        nbCgtDir++;
        turnLeft = true;
        turns[0]++;
    } else if(goodSortie == 2) {
        incS = 1;
        nbCgtDir++;
        turnRight = true;
        turns[1]++;
    }

    newDir = (int)direction + incS;
    if(newDir == -1) {
        newDir = NB_DIRECTION - 1;
    }
    if(newDir == NB_DIRECTION) {
        newDir = 0;
    }

    direction = (Game::Direction)newDir;

    if(snake.size() == 1) {
        queueDirection = direction;
    }
}

void Game::initReseau() {
    functions.clear();

    functions.append(Neurone::efSigmoide);
    functions.append(Neurone::efSigmoide);
    functions.append(Neurone::efSigmoide);
    functions.append(Neurone::efRelu);

    reseau.addCouche(Couche(24, 24, functions[0]));
    reseau.addCouche(Couche(24, 12, functions[1]));
    reseau.addCouche(Couche(12, 8, functions[2]));
    reseau.addCouche(Couche(8, 3, functions[3]));
}

void Game::getIncs(const Game::Direction& direction, int &incX, int &incY) const {
    switch(direction) {
    case Game::edHaut:
        incX = 0; incY = -1;
        return;
    case Game::edDroite:
        incX = 1; incY = 0;
        return;
    case Game::edBas:
        incX = 0; incY = 1;
        return;
    case Game::edGauche:
        incX = -1; incY = 0;
        return;
    }
}

Game::Direction Game::calculDirection(int incX, int incY) {
    if(incX == 0 && incY == -1) {
        return Game::edHaut;
    } else if(incX == 1 && incY == 0) {
        return Game::edDroite;
    } else if(incX == 0 && incY == 1) {
        return Game::edBas;
    } else {
        return Game::edGauche;
    }
}




