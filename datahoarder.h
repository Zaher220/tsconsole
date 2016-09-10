#ifndef DATAHOARDER_H
#define DATAHOARDER_H

#include <QObject>
#include "datatypes.h"
#include <QMutex>
#include <QMutexLocker>

class DataHoarder : public QObject
{
    Q_OBJECT
public:
    explicit DataHoarder(QObject *parent = 0);
signals:
    void sendADCData(ADCData);
public slots:
    void setADCData(ADCData raw);
    void reset();
    void uploadDataAndFree();
private:
    ADCData m_acqdata;
    const quint16 M_SUMPLES_NUM = 10;
    quint16 m_samples_num = 0;
    QMutex m_mutex;
};

#endif // DATAHOARDER_H
