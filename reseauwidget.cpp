#include "reseauwidget.h"

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
    QList<Couche>::const_iterator i;

    for(i=couches.begin();i!=couches.end();++i) {

    }
}
