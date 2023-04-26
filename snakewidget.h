#ifndef SNAKEWIDGET_H
#define SNAKEWIDGET_H

#include <QWidget>
#include <QPainter>

class SnakeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SnakeWidget(QWidget *parent = nullptr);
    void setDims(int largeur, int hauteur);
    void setSnake(const QList<QPoint>& snake);
    void setPasteque(const QPoint& pasteque);
    void showSensors(bool sensors);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    int largeur, hauteur;
    QList<QPoint> snake;
    QPoint pasteque;
    bool sensors;

    void drawGame(QPainter *painter);
};

#endif // SNAKEWIDGET_H
