#ifndef IOTON_H
#define IOTON_H

#include <QObject>
#include <QThread>
#include "ADCDataReader.h"
#include "datahoarder.h"
#include "analyze/signalanalyzer.h"
#include "analyze/volumevaluescalc.h"
#include "calibrator.h"

class Ioton : public QObject
{
    Q_OBJECT
public:
    explicit Ioton(QObject *parent = 0);
    ~Ioton();
    bool tryConnectToDevice();
signals:
    void deviceStatus(int status);
    void startMeas(int samples_num = 0);
    void stopMeas();
public slots:
    void beginResearch(int msecs = 0);
    void endResearch();
private:
    QThread m_thread;
    ADCDataReader *adc = nullptr;
    DataHoarder *hoarder = nullptr;
    SignalAnalyzer *analyzer = nullptr;
    VolumeValuesCalc *calc = nullptr;
    Calibrator *calib = nullptr;
};

#endif // IOTON_H
