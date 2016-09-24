#ifndef SIGNALCONVERTER_H
#define SIGNALCONVERTER_H

#include <QObject>
#include <QSettings>
#include "datatypes.h"

/*
Конвертер нужен для того чтобы выровнять нулевой сигнал от датчика объёма снятый в дискретах АЦП
с 0. На пример нуль датчика 1700, вычитаем из сигнала 1700.
*/
class SignalConverter : public QObject
{
    Q_OBJECT
public:
    explicit SignalConverter(QObject *parent = 0);
signals:
    void sendADCData(ADCData);
public slots:
    void setADCData(ADCData raw);
private:
    void loadSettings();
    double m_lift_coeff = 0;
};

#endif // SIGNALCONVERTER_H
