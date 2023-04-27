#ifndef SENSOR_H
#define SENSOR_H

#include <QPoint>

class Sensor {
public:
    typedef enum { estNone, estMur, estPasteque, estSnake } ESensorType;

    Sensor(const QPoint&p, ESensorType type);
    const QPoint& getPoint() const;
    const ESensorType& getType() const;
private:
    ESensorType type;
    QPoint p;
};

#endif // SENSOR_H
