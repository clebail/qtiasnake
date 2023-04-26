#include <QPainter>
#include "reseauwidget.h"

ReseauWidget::ReseauWidget(QWidget *parent) : QWidget{parent} {

}

void ReseauWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setPen(QColorConstants::White);
    painter.setBrush(QColorConstants::White);
    painter.drawRect(QRect(0, 0, width(), height()));
}
