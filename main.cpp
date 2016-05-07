#include <QCoreApplication>
#include "ADCDataReader.h"
#include "datahoarder.h"
#include "analyze/signalanalyzer.h"
#include "analyze/volumevaluescalc.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ADCDataReader adc;
    DataHoarder hoarder;
    SignalAnalyzer analyzer;
    VolumeValuesCalc calc;

    QObject::connect(&adc, SIGNAL(newData(ADCData)), &hoarder, SLOT(setADCData(ADCData)));
    QObject::connect(&hoarder, SIGNAL(sendADCData(ADCData)), &analyzer, SLOT(setADCData(ADCData)));
    //QObject::connect(&analyzer, SIGNAL(Inhalations(QVector<ing>)), &cacl, SLOT()
//    connect(&adc, SIGNAL(newData(ADCData)),
    return a.exec();
}
