#include "snakewidget.h"

SnakeWidget::SnakeWidget(QWidget *parent) : QWidget(parent) {
}

void SnakeWidget::setGame(const Game& game) {
    this->game = game;
    repaint();
}

void SnakeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setPen(QColorConstants::Black);
    painter.setBrush(QColorConstants::White);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    drawGame(&painter);
}

void SnakeWidget::drawGame(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing, true);

    int largeur = game.getLargeur();
    int hauteur = game.getHauteur();
    float caseWidth = (float)width() / (float)largeur;
    float caseHeight = (float)height() / (float)hauteur;
    float caseSize = qMin(caseWidth, caseHeight);
    float xMargin = ((float)width() - (float)(caseSize * largeur)) / 2.0;
    float yMargin = ((float)height() - (float)(caseSize * hauteur)) / 2.0;
    QList<QPoint> snake = game.getSnake();
    QPoint pasteque = game.getPasteque();

    // Murs (gris)
    painter->setPen(Qt::NoPen);
    for(int x=0;x<largeur;x++) {
        for(int y=0;y<hauteur;y++) {
            if(x == 0 || y == 0 || x == largeur - 1 || y == hauteur - 1) {
                QRectF cell(xMargin + x * caseSize, yMargin + y * caseSize, caseSize, caseSize);
                painter->fillRect(cell, QColor(110, 110, 120));
            }
        }
    }

    // Numéros sur les murs
    painter->setPen(QColorConstants::White);
    for(int x=0;x<largeur;x++) {
        for(int y=0;y<hauteur;y++) {
            if(x == 0 || y == 0 || x == largeur - 1 || y == hauteur - 1) {
                QRectF cell(xMargin + x * caseSize, yMargin + y * caseSize + 2, caseSize, caseSize);
                int n = (y == 0 || y == hauteur - 1) ? x : y;
                painter->drawText(cell, QString().number(n), QTextOption(Qt::AlignHCenter));
            }
        }
    }

    // Grille (traits noirs fins ; alpha réglable pour l'intensité)
    painter->setPen(QPen(QColor(0, 0, 0, 100), 1));
    for(int x=0;x<=largeur;x++) {
        float px = xMargin + x * caseSize;
        painter->drawLine(QPointF(px, yMargin), QPointF(px, yMargin + hauteur * caseSize));
    }
    for(int y=0;y<=hauteur;y++) {
        float py = yMargin + y * caseSize;
        painter->drawLine(QPointF(xMargin, py), QPointF(xMargin + largeur * caseSize, py));
    }

    // Serpent : segments arrondis dégradés, queue d'abord puis tête au-dessus
    float m = caseSize * 0.12;
    float seg = caseSize - 2 * m;
    float radius = seg * 0.35;
    for(int k = snake.size() - 1; k >= 0; k--) {
        QPoint p = snake[k];
        QRectF r(xMargin + p.x() * caseSize + m, yMargin + p.y() * caseSize + m, seg, seg);
        bool head = (k == 0);

        QLinearGradient g(r.topLeft(), r.bottomRight());
        g.setColorAt(0, head ? QColor(45, 170, 65) : QColor(65, 195, 85));
        g.setColorAt(1, head ? QColor(20, 110, 40) : QColor(30, 140, 55));

        painter->setPen(QPen(QColor(20, 90, 35), 1));
        painter->setBrush(g);
        painter->drawRoundedRect(r, radius, radius);

        if(head) {
            int dx = 0, dy = -1;
            if(snake.size() > 1) {
                dx = snake[0].x() - snake[1].x();
                dy = snake[0].y() - snake[1].y();
            }
            float eye = seg * 0.16;
            float fwd = seg * 0.20;
            float side = seg * 0.20;
            QPointF c = r.center();
            QPointF e1(c.x() + dx * fwd - dy * side, c.y() + dy * fwd + dx * side);
            QPointF e2(c.x() + dx * fwd + dy * side, c.y() + dy * fwd - dx * side);

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColorConstants::White);
            painter->drawEllipse(e1, eye, eye);
            painter->drawEllipse(e2, eye, eye);
            painter->setBrush(QColorConstants::Black);
            painter->drawEllipse(e1, eye * 0.5, eye * 0.5);
            painter->drawEllipse(e2, eye * 0.5, eye * 0.5);
        }
    }

    // Pastèque : tranche (écorce verte, chair rouge, pépins)
    if(!pasteque.isNull()) {
        float m2 = caseSize * 0.1;
        QRectF r(xMargin + pasteque.x() * caseSize + m2, yMargin + pasteque.y() * caseSize + m2, caseSize - 2 * m2, caseSize - 2 * m2);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(40, 140, 45));
        painter->drawEllipse(r);
        QRectF ri = r.adjusted(r.width() * 0.16, r.height() * 0.16, -r.width() * 0.16, -r.height() * 0.16);
        painter->setBrush(QColor(225, 55, 65));
        painter->drawEllipse(ri);

        painter->setBrush(QColorConstants::Black);
        float sd = qMax(1.0, r.width() * 0.06);
        painter->drawEllipse(QPointF(ri.center().x() - ri.width() * 0.15, ri.center().y() - ri.height() * 0.10), sd, sd);
        painter->drawEllipse(QPointF(ri.center().x() + ri.width() * 0.18, ri.center().y() - ri.height() * 0.05), sd, sd);
        painter->drawEllipse(QPointF(ri.center().x() - ri.width() * 0.02, ri.center().y() + ri.height() * 0.18), sd, sd);
    }
}
