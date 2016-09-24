#include "calibrator.h"

Calibrator::Calibrator(QObject *parent) : QObject(parent)
{
    this->loadSettings();
}

void Calibrator::signalAndParams(parameters params, ADCData data)
{
    m_params = params;
    m_params.debug();
    qDebug()<<"---------------------";
    this->calibrateVolumeParams();
}

void Calibrator::loadSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    m_volume_k = settings.value("/calibrations/volume_k", false).toDouble();
    m_temp_k1 = settings.value("/calibrations/temp_k1", false).toDouble();
    m_temp_k2 = settings.value("/calibrations/temp_k2", false).toDouble();
}

void Calibrator::calibrateVolumeParams()
{
    m_params.all_volume = fabs( m_params.all_volume / m_volume_k);
    m_params.av_speed = fabs( m_params.av_speed / m_volume_k);
    m_params.minute_volume = fabs( m_params.minute_volume / m_volume_k);
    m_params.max_speed = fabs( m_params.max_speed / m_volume_k);
    m_params.one_volume = fabs( m_params.one_volume / m_volume_k);
    m_params.debug();
}

void Calibrator::setVolume_coff(double value)
{
    m_volume_k = value;
}
