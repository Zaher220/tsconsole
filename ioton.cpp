#include "ioton.h"

Ioton::Ioton(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ADCData>("ADCData");

    this->moveToThread(&m_thread);
    adc = new ADCDataReader();
    hoarder = new DataHoarder();
    analyzer = new SignalAnalyzer();
    calc = new VolumeValuesCalc();
    calib = new Calibrator();

    connect(this, &Ioton::startMeas, adc, &ADCDataReader::startADC);
    connect(this, &Ioton::stopMeas, adc, &ADCDataReader::stopADC);

    connect(adc, &ADCDataReader::newData, hoarder, &DataHoarder::setADCData);
    connect(adc, &ADCDataReader::finished, hoarder, &DataHoarder::uploadDataAndFree);
    connect(hoarder, &DataHoarder::sendADCData, analyzer, &SignalAnalyzer::setADCData);
    connect(analyzer, &SignalAnalyzer::Inhalations, calc, &VolumeValuesCalc::setIngs);
    connect(calc, &VolumeValuesCalc::signalParameters, calib, &Calibrator::signalAndParams);

    adc->moveToThread(&m_thread);

    hoarder->moveToThread(&m_thread);
    analyzer->moveToThread(&m_thread);
    calc->moveToThread(&m_thread);
    calib->moveToThread(&m_thread);
//    m_analyze_thread.start();
//    m_adc_thread.start();
    m_thread.start();
}

Ioton::~Ioton()
{
    delete calib;
    delete calc;
    delete analyzer;
    delete hoarder;
    delete adc;
    m_thread.quit();
}

bool Ioton::tryConnectToDevice()
{
    return true;
}

void Ioton::beginResearch(int msecs)
{
    if( adc->isReady() )
        emit startMeas(msecs);
}

void Ioton::endResearch()
{
    emit stopMeas();
}

void Ioton::appendData(ADCData data)
{
    hoarder->setADCData(data);
}
