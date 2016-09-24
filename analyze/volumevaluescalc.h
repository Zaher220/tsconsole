#ifndef VOLUMEVALUESCALC_H
#define VOLUMEVALUESCALC_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include "datatypes.h"
/*!
    \brief Класс для расчёта объёмных параметров дыхания
    \author Алексеенко З.Н.
    \version 1.0
    \date 09.05.2016
    Класс предназначен для расчета параметров дыхания по объёму.
    На входе в класс поступает массив выдохов QVector<exhal> и  ADCData сырые  данные,
    собранные с ацп. На выходе структура  parameters считающая скорости вдоха выдоха.
*/

class VolumeValuesCalc : public QObject
{
    Q_OBJECT
public:
    explicit VolumeValuesCalc(QObject *parent = 0);
    parameters getParams();
    void setFrequancy(int Frequancy);
signals:
    void signalParameters(parameters, ADCData);
public slots:
    void setIngs(QVector<exhal> ings, ADCData data);
private:
    double av_speed(QVector<exhal> & ings);
    double max_speed(QVector<exhal> & ings);
    double one_volume(QVector<exhal> & ings);
    double minute_volume(QVector<exhal> & ings);
    double all_volume(QVector<exhal> & ings);
    double av_out_time(QVector<exhal> & ings);
    double av_in_time(QVector<exhal> & ings);
    double av_cycle_time(QVector<exhal> & ings);
    double freq(QVector<exhal> & ings);
    double av_temp_in(QVector<exhal> & ings, ADCData &data);
    double av_temp_out(QVector<exhal> & ings, ADCData &data);
    QVector<exhal> m_ings;
    ADCData m_acqdata;
    int m_Frequancy = 1000;
};

#endif // VOLUMEVALUESCALC_H
