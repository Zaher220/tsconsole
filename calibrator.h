#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QObject>

#include "datatypes.h"

class Calibrator : public QObject
{
    Q_OBJECT
public:
    explicit Calibrator(QObject *parent = 0);

signals:

public slots:
    void signalAndParams(parameters params, ADCData data);
};

#endif // CALIBRATOR_H
