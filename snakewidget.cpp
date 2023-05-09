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
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    drawGame(&painter);
}

void SnakeWidget::drawGame(QPainter *painter) {
    int largeur = game.getLargeur();
    int hauteur = game.getHauteur();
    float caseWidth = (float)width() / (float)largeur;
    float caseHeight = (float)height() / (float)hauteur;
    float caseSize = qMin(caseWidth, caseHeight);
    float xMargin = ((float)width() - (float)(caseSize * largeur)) / 2.0;
    float yMargin = ((float)height() - (float)(caseSize * hauteur)) / 2.0;
    QList<QPoint> snake = game.getSnake();
    QList<Sensor> sensors = game.getSensors();
    QPoint pasteque = game.getPasteque();
    QList<QPoint>::const_iterator i;
    QPoint tete;
    int diam = caseSize - 2;
    int dS = (caseSize - diam) / 2;

    painter->setPen(QColorConstants::Black);
    for(int x=0;x<largeur;x++) {
        for(int y=0;y<hauteur;y++) {
            if(x == 0 || y == 0 || x == largeur - 1 || y == hauteur - 1) {
               painter->setBrush(QColorConstants::Gray);

               painter->drawRect(QRectF(xMargin + x * caseSize, yMargin + y * caseSize, caseSize, caseSize));
               if(y == 0 || y == hauteur - 1) {
                   painter->drawText(QRectF(xMargin + x * caseSize, yMargin + y * caseSize + 2, caseSize, caseSize), QString().number(x), QTextOption(Qt::AlignHCenter));
               } else {
                   painter->drawText(QRectF(xMargin + x * caseSize, yMargin + y * caseSize + 2, caseSize, caseSize), QString().number(y), QTextOption(Qt::AlignHCenter));
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

        painter->drawEllipse(xMargin + p.x() * caseSize + dS , yMargin + p.y() * caseSize + dS, diam, diam);
    }

    if(!pasteque.isNull()) {
        QColor color = QColorConstants::DarkGreen;

        painter->setPen(color);
        painter->setBrush(color);

        painter->drawEllipse(xMargin + pasteque.x() * caseSize + dS, yMargin + pasteque.y() * caseSize + dS, diam, diam);
    }

    if(!tete.isNull() && this->sensors && sensors.size()) {
        int caseSize2 = caseSize / 2;
        int startX = xMargin + tete.x() * caseSize + caseSize2;
        int startY = yMargin + tete.y() * caseSize + caseSize2;
        QPoint start(startX, startY);

        for(int i=0;i<sensors.size();i++) {
            QPoint p = sensors[i].getPoint();
            if(!p.isNull()) {
                QPoint p(xMargin + sensors[i].getPoint().x() * caseSize + caseSize2, yMargin + sensors[i].getPoint().y() * caseSize + caseSize2);
                QColor color = sensors[i].getType() == Sensor::estPasteque ? QColorConstants::Green : (sensors[i].getType() == Sensor::estSnake ? QColorConstants::Black : QColorConstants::Gray);

                painter->setPen(color);
                painter->drawLine(start, p);
            }
        }
    }
}
