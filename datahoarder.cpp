#include "datahoarder.h"

DataHoarder::DataHoarder(QObject *parent) : QObject(parent)
{

}

void DataHoarder::setADCData(ADCData raw)
{
    m_acqdata += raw;
    emit sendADCData(m_acqdata);
}

void DataHoarder::reset()
{
    m_acqdata.clear();
}
