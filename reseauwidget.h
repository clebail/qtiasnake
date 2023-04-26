#ifndef RESEAUWIDGET_H
#define RESEAUWIDGET_H

#include <QWidget>
#include <QPainter>
#include "reseau.h"

class ReseauWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ReseauWidget(QWidget *parent = nullptr);
    void setReseau(const Reseau& reseau);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    Reseau reseau;

    void drawReseau(QPainter *painter);

};

#endif // RESEAUWIDGET_H
