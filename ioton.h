#ifndef IOTON_H
#define IOTON_H

#include <QObject>
#include <QThread>
#include "ADCDataReader.h"
#include "datahoarder.h"
#include "analyze/signalanalyzer.h"
#include "analyze/volumevaluescalc.h"
#include "signalconverter.h"
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
    void appendData(ADCData data);
private:
    QThread m_thread, m_adc_thread, m_analyze_thread;
    ADCDataReader *adc = nullptr;
    SignalConverter *m_signal_converter;
    DataHoarder *m_hoarder = nullptr;
    SignalAnalyzer *m_analyzer = nullptr;
    VolumeValuesCalc *m_calculator = nullptr;
    Calibrator *m_calibrator = nullptr;

};

#endif // IOTON_H
