#include <QtDebug>
#include "reseauwidget.h"

#define DIAM            8
#define RAYON           (DIAM / 2)
#define MARGE           55
#define ROW_H           20

ReseauWidget::ReseauWidget(QWidget *parent) : QWidget{parent} {
}

void ReseauWidget::setReseau(const Reseau& reseau) {
    this->reseau = reseau;
    repaint();
}

void ReseauWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setPen(QColorConstants::White);
    painter.setBrush(QColorConstants::White);
    painter.drawRect(QRect(0, 0, width(), height()));

    drawReseau(&painter);
}

void ReseauWidget::drawReseau(QPainter *painter) {
    QList<Couche> couches = reseau.getCouches();
    QList<float> entrees = reseau.getEntress();
    QList<float> sorties = reseau.getSorties();

    if(couches.isEmpty())
        return;

    // Une colonne pour les entrées + une par couche.
    int nbCol = couches.size() + 1;
    float colStep = (nbCol > 1) ? (width() - 2 * MARGE) / (float)(nbCol - 1) : 0;

    QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);
    painter->setPen(QColorConstants::Black);

    // Colonne des entrées : nœud + valeur alignés 1:1 et centrés verticalement.
    // Couleur divergente : blanc à 0, vert vers +1, rouge vers -1.
    float entreesHeight = height() / (float)(entrees.size() + 1);
    for(int i=0;i<entrees.size();i++) {
        int y = (i + 1) * entreesHeight;

        float v = qBound(-1.0f, entrees[i], 1.0f);
        QColor col;
        if(v >= 0) {
            col = QColor(255 - (int)(255 * v), 255 - (int)(127 * v), 255 - (int)(255 * v));
        } else {
            float a = -v;
            col = QColor(255 - (int)(55 * a), 255 - (int)(255 * a), 255 - (int)(255 * a));
        }

        painter->setBrush(col);
        painter->drawEllipse(QPoint(MARGE, y), RAYON, RAYON);
        painter->drawText(QRect(0, y - ROW_H / 2, MARGE - DIAM, ROW_H),
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString().number((double)entrees[i], 'f', 3));
    }

    // Couches cachées et de sortie.
    for(int i=0;i<couches.size();i++) {
        QList<Neurone> neurones = couches[i].getNeurones();

        if(neurones.isEmpty())
            continue;

        int x = MARGE + (int)((i + 1) * colStep);
        float neuroneHeight = height() / (float)(neurones.size() + 1);

        for(int j=0;j<neurones.size();j++) {
            int y = (j + 1) * neuroneHeight;

            painter->setBrush(neurones[j].getValue() >= SEUIL ? QColorConstants::DarkGreen : QColorConstants::White);
            painter->drawEllipse(QPoint(x, y), RAYON, RAYON);
        }
    }

    // Valeurs de sortie, alignées sur les nœuds de la dernière couche.
    float sortiesHeight = height() / (float)(sorties.size() + 1);
    for(int i=0;i<sorties.size();i++) {
        int y = (i + 1) * sortiesHeight;

        painter->drawText(QRect(width() - MARGE + DIAM, y - ROW_H / 2, MARGE - DIAM, ROW_H),
                          Qt::AlignLeft | Qt::AlignVCenter,
                          QString().number((double)sorties[i], 'f', 3));
    }
}
