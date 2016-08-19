#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QObject>

#include "datatypes.h"

class Calibrator : public QObject
{
    Q_OBJECT
public:
    explicit Calibrator(QObject *parent = 0);
    void setVolume_coff(double value);
signals:

public slots:
    void signalAndParams(parameters params, ADCData data);
private:
    void calibrateVolumeParams();
    double volume_coff = 83047.4;//1540;//296675
    parameters m_params;
};

#endif // CALIBRATOR_H
