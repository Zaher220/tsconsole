#ifndef DATAHOARDER_H
#define DATAHOARDER_H

#include <QObject>
#include "datatypes.h"

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
private:
    ADCData m_acqdata;
};

#endif // DATAHOARDER_H
