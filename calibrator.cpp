#include "calibrator.h"

Calibrator::Calibrator(QObject *parent) : QObject(parent)
{

}

void Calibrator::signalAndParams(parameters params, ADCData data)
{
    m_params = params;
    m_params.debug();
    qDebug()<<"---------------------";
    this->calibrateVolumeParams();
}

void Calibrator::calibrateVolumeParams()
{
    m_params.all_volume = fabs( m_params.all_volume / volume_coff);
    m_params.av_speed = fabs( m_params.av_speed / volume_coff);
    m_params.minute_volume = fabs( m_params.minute_volume / volume_coff);
    m_params.max_speed = fabs( m_params.max_speed / volume_coff);
    m_params.one_volume = fabs( m_params.one_volume / volume_coff);
    m_params.debug();
}
