#include "signalconverter.h"

SignalConverter::SignalConverter(QObject *parent) : QObject(parent)
{
    this->loadSettings();
}

void SignalConverter::setADCData(ADCData raw)
{
    for(auto i = 0; i < raw.DATA_LEN; i++){
        for(auto k = 0; k < raw.data[i].size();k++)
            raw.data[i][k] = raw.data[i][k] + m_lift_coeff;
    }
    emit sendADCData(raw);
}

void SignalConverter::loadSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    m_lift_coeff = settings.value("/calibrations/signal_lift", false).toDouble();
}
