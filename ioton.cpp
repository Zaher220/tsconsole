#include "ioton.h"

Ioton::Ioton(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ADCData>("ADCData");

    this->moveToThread(&m_thread);
    adc = new ADCDataReader();
    m_signal_converter = new SignalConverter();
    m_hoarder = new DataHoarder();
    m_analyzer = new SignalAnalyzer();
    m_calculator = new VolumeValuesCalc();
    m_calibrator = new Calibrator();

    connect(this, &Ioton::startMeas, adc, &ADCDataReader::startADC);
    connect(this, &Ioton::stopMeas, adc, &ADCDataReader::stopADC);

    connect(adc, &ADCDataReader::newData, m_signal_converter, &SignalConverter::setADCData);
    connect(m_signal_converter, &SignalConverter::sendADCData, m_hoarder , &DataHoarder::setADCData);
    connect(adc, &ADCDataReader::finished, m_hoarder, &DataHoarder::uploadDataAndFree);
    connect(m_hoarder, &DataHoarder::sendADCData, m_analyzer, &SignalAnalyzer::setADCData);
    connect(m_analyzer, &SignalAnalyzer::Inhalations, m_calculator, &VolumeValuesCalc::setIngs);
    connect(m_calculator, &VolumeValuesCalc::signalParameters, m_calibrator, &Calibrator::signalAndParams);

    adc->moveToThread(&m_thread);

    m_hoarder->moveToThread(&m_thread);
    m_analyzer->moveToThread(&m_thread);
    m_calculator->moveToThread(&m_thread);
    m_calibrator->moveToThread(&m_thread);
//    m_analyze_thread.start();
//    m_adc_thread.start();
    m_thread.start();
}

Ioton::~Ioton()
{
    delete m_calibrator;
    delete m_calculator;
    delete m_analyzer;
    delete m_hoarder;
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
    m_signal_converter->setADCData(data);
    //m_hoarder->setADCData(data);
}
