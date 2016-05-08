#ifndef SIGNALANALYZER_H
#define SIGNALANALYZER_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QTime>
#include "../datatypes.h"

class SignalAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit SignalAnalyzer(QObject *parent = 0);
    QVector<double> getMedian();
    QVector<double> getClear();
    QVector<double> getIntegrated();
    QVector<double> getAreas();
    void setFinderPrefs(FinderPrefs prefs);    
signals:
    void Inhalations(QVector<ing>, ADCData);
public slots:
    void setADCData(ADCData data);
    void addRawData(QVector<int> * signal);
    void addMultiplyRawData(QVector<int> volume, QVector<int> tempin, QVector<int> tempout);
    void setFullPatientData(VTT_Data data);
private:
    void reset();
    QVector<double> median(QVector<int> *signal, int period);
    QVector<double> clearSignal(QVector<double> signal);
    void integrateSignal(int start);
    void findIng(int start);
    double area(double a, double b);
    QVector<int> m_raw_signal;//чистый сигнал
    QVector<double> m_median_signal;//сигнал пропущенный через медианный фильтр
    QVector<double> m_clean_signal;//сигнал после фильтра и с занулёнными промежетьками когда человек вдыхает
    QVector<double> m_int_signal;
    QVector<ing> m_ings;
    int period = 15;
    int zero_level = 1540;
    int zero_sigma = 15;
    ADCData m_adc_data;
};

#endif // SIGNALANALYZER_H
