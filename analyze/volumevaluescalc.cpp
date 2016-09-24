#include "volumevaluescalc.h"

VolumeValuesCalc::VolumeValuesCalc(QObject *parent) : QObject(parent)
{

}

void VolumeValuesCalc::setIngs(QVector<exhal> ings, ADCData data)
{
    m_ings = ings;
    m_acqdata = data;
    emit signalParameters(this->getParams(), data);
}

parameters VolumeValuesCalc::getParams()
{
    parameters ps;
    if(m_ings.size() == 0)
        return ps;
    ps.all_volume = all_volume(m_ings);
    ps.av_speed = av_speed(m_ings);
    ps.max_speed = this->max_speed(m_ings);
    ps.one_volume = this->one_volume(m_ings);
    ps.minute_volume = this->minute_volume(m_ings);
    ps.freq = this->freq(m_ings);
    ps.av_in_time = this->av_in_time(m_ings);
    ps.av_out_time = this->av_out_time(m_ings);
    ps.av_cycle_time = this->av_cycle_time(m_ings);
    ps.av_temp_in = this->av_temp_in(m_ings, m_acqdata);
    ps.av_temp_out = this->av_temp_out(m_ings, m_acqdata);
    return ps;
}

double VolumeValuesCalc::av_speed(QVector<exhal> &ings)
{
    double average_speed = 0;
    double speed = 0;
    for(int i = 0; i < ings.size(); i++){
        speed = ings[i].area / (ings.at(i).end_index - ings.at(i).start_index);
        average_speed = ((average_speed * i) + speed) / (i + 1);
    }
    average_speed = average_speed * m_Frequancy;//Преобразуем в единицы объёма в секунду

    return average_speed;
}

double VolumeValuesCalc::max_speed(QVector<exhal> &ings)
{
    double maximum_speed = 0;
    double speed = 0;
    for(int i = 0; i < ings.size(); i++){
        speed = ings[i].area / (ings.at(i).end_index - ings.at(i).start_index);
        if( fabs(speed) > fabs(maximum_speed) )
            maximum_speed = speed;
    }
    maximum_speed = maximum_speed * m_Frequancy;//Преобразуем в единицы объёма в секунду

    return maximum_speed;
}

double VolumeValuesCalc::one_volume(QVector<exhal> &ings)//Средний дыхательный объём
{
    double average_volume = 0;
    double volume = 0;
    for(int i = 0; i < ings.size(); i++){
        volume = ings[i].area;
        average_volume = ((average_volume * i) + volume) / (i + 1);
    }
    return average_volume;
}

double VolumeValuesCalc::minute_volume(QVector<exhal> &ings)
{
    double summ_volume = this->all_volume(ings);
    double full_time =  ings.last().end_index - ings.first().start_index;
    double minute_vol = summ_volume / ( full_time / (m_Frequancy * 60) );
    //qDebug()<<"++ minute_vol "<<(double)minute_vol / 82000; ///83047.4;
    return minute_vol;
}

double VolumeValuesCalc::all_volume(QVector<exhal> &ings)
{
    double summ = 0;
    foreach (exhal i, ings) {
        summ += i.area;
    }
    return summ;
}

double VolumeValuesCalc::av_out_time(QVector<exhal> &ings)
{
    double average_time = 0;
    double one_time = 0;
    for(int i = 0; i < ings.size(); i++){
        one_time = ings[i].end_index - ings[i].start_index;
        average_time = ((average_time * i) + one_time) / (i + 1);
    }
    average_time = average_time / m_Frequancy;//Преобразуем такты ацп в секунды
    return average_time;
}

double VolumeValuesCalc::av_in_time(QVector<exhal> &ings)
{
    return this->av_cycle_time(ings) - this->av_out_time(ings);
}

double VolumeValuesCalc::av_cycle_time(QVector<exhal> &ings)
{
    double average_time = 0;
    double one_time = 0;
    for(size_t i = 0; i < ings.size() - 1; i++){
        one_time = ings[i + 1].max_index - ings[i].max_index;
        average_time = ((average_time * i) + one_time) / (i + 1);
    }
    average_time = average_time / m_Frequancy;//Преобразуем такты ацп в секунды
    return average_time;

    return -1;
}

double VolumeValuesCalc::freq(QVector<exhal> &ings)//Частота дыхания единицы в минуту
{
    double inh_number =  ings.size();
    double full_time =  ings.last().end_index - ings.first().start_index;
    double inh_freq = inh_number / ( full_time / (m_Frequancy * 60) );
    //qDebug()<<"++ inh_freq "<<(double)inh_freq;
    return inh_freq;
}

double VolumeValuesCalc::av_temp_in(QVector<exhal> & ings, ADCData &data)
{
    //Проверим что количество данных температуры и давления совпадают и не равны 0
    if( data.data[1].size() == data.data[0].size() && data.data[2].size() > 0 ){
        double average_tempin = 0;
        double one_temp = 0;
        for(int i = 0; i < ings.size(); i++){
            one_temp = data.data[1].at(ings[i].start_index);
            average_tempin = ((average_tempin * i) + one_temp) / (i + 1);
        }
        return average_tempin;
    }
    return -1;
}

double VolumeValuesCalc::av_temp_out(QVector<exhal> & ings, ADCData &data)
{
    //Проверим что количество данных температуры и давления совпадают и не равны 0
    if( data.data[2].size() == data.data[0].size() && data.data[2].size() > 0 ){
        double average_tempout = 0;
        double one_temp = 0;
        for(int i = 0; i < ings.size(); i++){
            one_temp = data.data[2].at(ings[i].end_index);
            average_tempout = ((average_tempout * i) + one_temp) / (i + 1);
        }
        return average_tempout;
    }
    return -1;
}

void VolumeValuesCalc::setFrequancy(int Frequancy)
{
    m_Frequancy = Frequancy;
}
