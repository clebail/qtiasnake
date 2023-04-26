#include <math.h>
#include "snakewidget.h"

#ifndef PI
    #define PI ((float)3.14159)
#endif

#define PI4 ((float)PI/4.0)

SnakeWidget::SnakeWidget(QWidget *parent) : QWidget(parent) {
    largeur = 12;
    hauteur = 12;
    sensors = true;
}

void SnakeWidget::setDims(int largeur, int hauteur) {
    this->largeur = largeur;
    this->hauteur = hauteur;

    repaint();
}

void SnakeWidget::setSnake(const QList<QPoint>& snake) {
    this->snake = snake;
    repaint();
}

void SnakeWidget::setPasteque(const QPoint& pasteque) {
    this->pasteque = pasteque;
    repaint();
}

void SnakeWidget::showSensors(bool sensors) {
    if(this->sensors != sensors) {
        this->sensors = sensors;
        repaint();
    }
}

void SnakeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setPen(QColorConstants::Black);
    painter.setBrush(QColorConstants::White);
    painter.drawRect(QRect(0, 0, width(), height()));

    drawGame(&painter);
}

void SnakeWidget::drawGame(QPainter *painter) {
    float caseWidth = (float)width() / (float)largeur;
    float caseHeight = (float)height() / (float)hauteur;
    QList<QPoint>::const_iterator i;
    QPoint tete;

    painter->setPen(QColorConstants::Black);
    for(int x=0;x<largeur;x++) {
        for(int y=0;y<hauteur;y++) {
            if(x == 0 || y == 0 || x == largeur - 1 || y == hauteur - 1) {
               painter->setBrush(QColorConstants::Gray);

               painter->drawRect(QRectF(x * caseWidth, y * caseHeight, caseWidth, caseHeight));
            }
        }
    }

    for(i=snake.begin();i!=snake.end();++i) {
        QColor bColor = QColorConstants::Black;
        QColor pColor = QColorConstants::White;
        QPoint p = *i;

        if(i == snake.begin()) {
            tete = *i;
            bColor = QColorConstants::DarkRed;
            pColor = QColorConstants::DarkRed;
        }

        painter->setPen(pColor);
        painter->setBrush(bColor);

        painter->drawRect(QRectF(p.x() * caseWidth, p.y() * caseHeight, caseWidth, caseHeight));
    }

    if(!pasteque.isNull()) {
        QColor color = QColorConstants::DarkGreen;

        painter->setPen(color);
        painter->setBrush(color);

        painter->drawRect(QRectF(pasteque.x() * caseWidth, pasteque.y() * caseHeight, caseWidth, caseHeight));
    }

    if(sensors && !tete.isNull()) {
        int caseWidth2 = caseWidth / 2;
        int caseHeight2 = caseHeight / 2;
        int startX = tete.x() * caseWidth + caseWidth2;
        int startY =tete.y() * caseHeight + caseHeight2;
        int minX = caseWidth2;
        int minY = caseHeight2;
        int maxX = largeur * caseWidth - caseWidth2;
        int maxY = hauteur * caseHeight - caseHeight2;
        QPoint start(startX, startY);

        painter->setPen(QColorConstants::Blue);
        painter->drawLine(start, QPoint(startX, minY));
        painter->drawLine(start, QPoint(startX, maxY));
        painter->drawLine(start, QPoint(minX, startY));
        painter->drawLine(start, QPoint(maxX, startY));
        painter->drawLine(start, QPoint(startX * cos(5*PI4) + startX, startY * sin(5*PI4) + startY));
    }
}
