#include "sensor.h"

Sensor::Sensor(const QPoint&p, Sensor::ESensorType type) {
    this->p = p;
    this->type = type;
}

const QPoint& Sensor::getPoint() const {
    return p;
}

const Sensor::ESensorType& Sensor::getType() const {
    return type;
}
