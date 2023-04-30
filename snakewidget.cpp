#include "snakewidget.h"

SnakeWidget::SnakeWidget(QWidget *parent) : QWidget(parent) {
   sensors = true;
}

void SnakeWidget::setGame(const Game& game) {
    this->game = game;
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
    int largeur = game.getLargeur();
    int hauteur = game.getHauteur();
    float caseWidth = (float)width() / (float)largeur;
    float caseHeight = (float)height() / (float)hauteur;
    QList<QPoint> snake = game.getSnake();
    QList<Sensor> sensors = game.getSensors();
    QPoint pasteque = game.getPasteque();
    QList<QPoint>::const_iterator i;
    QPoint tete;
    int diam = qMin(caseHeight, caseWidth) - 2;
    int dX = (caseWidth - diam) / 2;
    int dY = (caseHeight - diam) / 2;

    painter->setPen(QColorConstants::Black);
    for(int x=0;x<largeur;x++) {
        for(int y=0;y<hauteur;y++) {
            if(x == 0 || y == 0 || x == largeur - 1 || y == hauteur - 1) {
               painter->setBrush(QColorConstants::Gray);

               painter->drawRect(QRectF(x * caseWidth, y * caseHeight, caseWidth, caseHeight));
               if(y == 0 || y == hauteur - 1) {
                   painter->drawText(QRectF(x * caseWidth, y * caseHeight, caseWidth, caseHeight), QString().number(x), QTextOption(Qt::AlignHCenter));
               } else {
                   painter->drawText(QRectF(x * caseWidth, y * caseHeight, caseWidth, caseHeight), QString().number(y), QTextOption(Qt::AlignHCenter));
               }
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

        painter->drawEllipse(p.x() * caseWidth + dX , p.y() * caseHeight + dY, diam, diam);
    }

    if(!pasteque.isNull()) {
        QColor color = QColorConstants::DarkGreen;

        painter->setPen(color);
        painter->setBrush(color);

        painter->drawEllipse(pasteque.x() * caseWidth + dX, pasteque.y() * caseHeight + dY, diam, diam);
    }

    if(!tete.isNull() && this->sensors && sensors.size()) {
        int caseWidth2 = caseWidth / 2;
        int caseHeight2 = caseHeight / 2;
        int startX = tete.x() * caseWidth + caseWidth2;
        int startY =tete.y() * caseHeight + caseHeight2;
        QPoint start(startX, startY);
        QList<Sensor>::const_iterator i;

        painter->setPen(QColorConstants::Blue);
        for(i=sensors.begin();i!=sensors.end();++i) {
            QPoint p((*i).getPoint().x() * caseWidth + caseWidth2, (*i).getPoint().y() * caseHeight + caseHeight2);

            painter->drawLine(start, p);
        }
    }
}
