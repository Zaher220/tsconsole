#include "datahoarder.h"

DataHoarder::DataHoarder(QObject *parent) : QObject(parent)
{

}

void DataHoarder::setADCData(ADCData raw)
{
    //QMutexLocker locker(&m_mutex);
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
    FILE * out = nullptr;
    out = fopen("raw_data.csv","w");

    for(int i = 0; i < m_acqdata.data[0].size();i++){
        for(int k = 0; k < m_acqdata.DATA_LEN; k++){
            fprintf(out,"%d;",m_acqdata.data[k][i]);
        }
        fprintf(out,"\n");
    }

    if( out != NULL && out != nullptr )
        fclose(out);
    emit sendADCData(m_acqdata);
    qDebug()<<"Finish";
    m_acqdata.clear();
}
