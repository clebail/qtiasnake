#include <QtDebug>
#include "reseauwidget.h"

#define DIAM            20
#define RAYON           (DIAM / 2)

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
    if(couches.size()) {
        QList<Couche>::const_iterator c;
        int i;
        int coucheWidth = width() / (couches.size() + 1);

        painter->setPen(QColorConstants::Black);
        for(i=0,c=couches.begin();c!=couches.end();++c, i++) {
            Couche couche = *c;
            QList<Neurone> neurones = couche.getNeurones();

            if(neurones.size()) {
                QList<Neurone>::const_iterator n;
                int x = i * coucheWidth + coucheWidth - RAYON;
                int j;
                float neuroneHeight = height() / (neurones.size() + 1);

                for(j=0, n=neurones.begin();n!=neurones.end();++n, j++) {
                    Neurone neurone = *n;
                    int y = j * neuroneHeight + neuroneHeight - RAYON;

                    painter->setBrush(neurone.getValue() >= 0.5 ? QColorConstants::DarkGreen : QColorConstants::White);
                    painter->drawEllipse(x, y, DIAM, DIAM);
                }
            }
        }
    }
}
