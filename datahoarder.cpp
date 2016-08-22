#include "datahoarder.h"

DataHoarder::DataHoarder(QObject *parent) : QObject(parent)
{

}

void DataHoarder::setADCData(ADCData raw)
{
    m_acqdata += raw;
    if( m_samples_num%M_SUMPLES_NUM == 0 ){
        emit sendADCData(m_acqdata);
        m_samples_num = 0;
    }
    m_samples_num++;
}

void DataHoarder::reset()
{
    m_acqdata.clear();
}

void DataHoarder::uploadDataAndFree()
{
    emit sendADCData(m_acqdata);
    m_acqdata.clear();
}
