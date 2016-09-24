#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QObject>
#include <QSettings>
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
    void loadSettings();
    void calibrateVolumeParams();
    //double volume_coff = 586795; //83047.4;//1540;//296675
    double m_volume_k = 0;
    double m_temp_k1 = 0;
    double m_temp_k2 = 0;
    parameters m_params;
};

#endif // CALIBRATOR_H
