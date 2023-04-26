#ifndef RESEAUWIDGET_H
#define RESEAUWIDGET_H

#include <QWidget>

class ReseauWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ReseauWidget(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *);

};

#endif // RESEAUWIDGET_H
