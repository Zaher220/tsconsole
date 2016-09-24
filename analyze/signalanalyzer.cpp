#include "signalanalyzer.h"

SignalAnalyzer::SignalAnalyzer(QObject *parent) : QObject(parent)
{

}

QVector<double> SignalAnalyzer::getMedian()
{
    return m_median_signal;
}

QVector<double> SignalAnalyzer::getClear()
{
    return m_clean_signal;
}

QVector<double> SignalAnalyzer::getIntegrated()
{
    return m_int_signal;
}

QVector<double> SignalAnalyzer::getAreas()
{
    QVector<double> res;
    for(auto & i: m_ings){
        res.push_back(i.area);
    }
    return res;
}

void SignalAnalyzer::setFinderPrefs(FinderPrefs prefs)
{
    m_period = prefs.median_period;
    zero_level = prefs.zero_level;
    zero_sigma = prefs.zero_sigma;
}

void SignalAnalyzer::setADCData(ADCData data)
{
    m_adc_data = data;
    if( m_adc_data.data[0].size() != 0 ){
        this->addMultiplyRawData(m_adc_data.data[0], m_adc_data.data[0], m_adc_data.data[0]);
    }
}

void SignalAnalyzer::addRawData(QVector<int> *signal)
{
    int start = 0;
    m_raw_signal = m_adc_data.data[0];
    m_median_signal = median(m_raw_signal, m_period);


//    FILE * out = nullptr;
//    out = fopen("median.csv", "w");

//    for(int i = 0; i < m_median_signal.size();i++){
//        fprintf(out,"%f\n", m_median_signal.at(i));
//    }

//    if( out != NULL && out != nullptr )
//        fclose(out);

    m_clean_signal = this->clearSignal(m_median_signal);
    qDebug()<<"m_raw_signal.size() "<<m_raw_signal.size();
    qDebug()<<"m_median_signal.size() "<<m_median_signal.size();
    qDebug()<<"m_clean_signal.size() "<<m_clean_signal.size();
    this->findExhalations(start);

    emit Inhalations(m_ings, m_adc_data);
}

void SignalAnalyzer::addMultiplyRawData(QVector<int> volume, QVector<int> tempin, QVector<int> tempout)
{
    this->addRawData(&volume);
}

void SignalAnalyzer::setFullPatientData(VTT_Data data)
{
    this->reset();
    QVector<int> vec;
    for(int i=0;i<data.volume.size();i++)
        vec.push_back(data.volume.at(i)+zero_level);
    this->addRawData(&vec);
}

void SignalAnalyzer::reset()
{
    m_raw_signal.clear();
    m_median_signal.clear();
    m_clean_signal.clear();
    m_int_signal.clear();
    m_ings.clear();
}

QVector<double> SignalAnalyzer::median(const QVector<int> signal, const size_t period)
{
    double med = 0;//знаение медианы
    QVector<int> temp;//Временный массив для сортировки данных которым расчитывается медиана
    temp.reserve(static_cast<int>(period));
    QVector<double> result;
    result.reserve(signal.size());

    for(size_t i = period; i < static_cast<size_t>(signal.size()); i++){
        for(size_t k = i - period; k < i; k++)
            temp.push_back(signal.at(static_cast<int>(k)));
        qSort(temp);
        if( period % 2 == 0 )
            med = static_cast<double>(temp.at(period / 2) + temp.at(period / 2 + 1)) / 2;
        else
            med = temp.at(period / 2 + 1);
        result.push_back(med);
        temp.clear();
    }
    return result;
}

QVector<double> SignalAnalyzer::clearSignal(QVector<double> signal)/*Удалим нули т.е. промежутки когда человек не выдыхает*/
{
    QVector<double> clean_signal;
    clean_signal.reserve(signal.size());

    for(size_t i = 0; i < static_cast<size_t>(m_median_signal.size()); i++){
        if( zero_level - m_median_signal.at(i)  < 0 || abs(zero_level - m_median_signal.at(i)) < zero_sigma ){//всё что в окрестности нулевого уровня или выше прировняем к 0 уровню
            clean_signal.push_back( zero_level );
        }else{
            clean_signal.push_back( m_median_signal.at(i) );
        }
        clean_signal[i] -= zero_level;
    }
    return clean_signal;
}

void SignalAnalyzer::integrateSignal(size_t start)
{
    m_int_signal.erase(m_int_signal.begin() + start, m_int_signal.end());
    for(size_t i = start; i < m_clean_signal.size(); i++){
        int a = m_clean_signal[i];
        int b = 0;
        /*if ( m_int_signal.size() == 1 )
            m_int_signal[0];*/
        if ( m_int_signal.size() > 0 ){
            b = m_clean_signal[i-1];
            m_int_signal.push_back(area(a,b));
        }else{
            m_int_signal.push_back(0);
        }
    }

}

void SignalAnalyzer::findExhalations(size_t start)
{
    QMutexLocker locker(&m_mutex);
    auto findStartIndex = [](QVector<double> * data, size_t start_pos, size_t zero_count ){
        if( data->size() - (int)zero_count < 0 )
            return -1;
        for(size_t i = start_pos; i < static_cast<size_t>(data->size()) - zero_count; i++){
            bool is_zero_seq = true;
            for(size_t k = i; k < i + zero_count -1; k++){
                if( fabs( data->at(k)) > 0.0000001 )
                    is_zero_seq = false;
            }
            if( is_zero_seq )
                return static_cast<int>(i);
        }
        return -1;
    };

    auto findZeroSeqEnd = [](QVector<double> * data, size_t start_pos){
        for(size_t i = start_pos; i < static_cast<size_t>(data->size()); i++){
            if( fabs( data->at(i)) > 0.0 )
                return static_cast<int>(i);
        }
        return -1;
    };

    auto findMaxIndex = [](QVector<double> * data, size_t start_pos, size_t end_pos){
        size_t maxindex = start_pos;
        for(size_t i = start_pos; i < end_pos; i++){
            if( fabs( data->at(i)) > fabs( data->at(maxindex)) )
                maxindex = i;
        }
        return maxindex;
    };

    auto countSumm = [](QVector<double> * data, size_t start_pos, size_t end_pos){
        double summ = 0;
        for(size_t i = start_pos; i < end_pos; i++){
            summ += data->at(i);
        }
        return summ;
    };


    int start_pos = start;
    int zero_seq_leh = 5;

    int zeros_start = 0;
    int zeros_end = 0;
    int max_index = 0;
    int ind_end = 0;

    double summ = 0;
    exhal ingal;
    while(true){
        zeros_start = findStartIndex(&m_clean_signal, start_pos, zero_seq_leh); //найдем последовательность нулей
        if( zeros_start == -1)
            return;
        zeros_end = findZeroSeqEnd(&m_clean_signal, zeros_start);//найдём окончание последовательности нулей это и будет начало выдоха
        if( zeros_end ==-1 )
            return;

        ind_end = findStartIndex(&m_clean_signal, zeros_end, zero_seq_leh);// найдём окончание выдоха
        if( ind_end == -1 )
            return;


        summ = countSumm(&m_clean_signal, zeros_end, ind_end);
        max_index = findMaxIndex(&m_clean_signal, zeros_end, ind_end);

        ingal.start_index = zeros_end;
        ingal.end_index = ind_end;
        ingal.max_index = max_index;
        ingal.area = summ;
        m_ings.push_back(ingal);

        start_pos = ind_end;

    }
}

double SignalAnalyzer::area(double a, double b)
{
    return (a+b)/2;
}
