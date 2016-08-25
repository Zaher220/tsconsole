#include <QCoreApplication>
#include <QDebug>

#include "ioton.h"
#include "ADCDataReader.h"
#include "datahoarder.h"
#include "analyze/signalanalyzer.h"
#include "analyze/volumevaluescalc.h"
#include "calibrator.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Ioton ioton;
    ioton.beginResearch(50000);
    qDebug()<<"before sleep";
//    Sleep(5000);
//    qDebug()<<"after sleep";
//    ioton.endResearch();
//    ADCDataReader adc;
//    DataHoarder hoarder;
//    SignalAnalyzer analyzer;
//    VolumeValuesCalc calc;
//    Calibrator calib;

//    QObject::connect(&adc, SIGNAL(newData(ADCData)), &hoarder, SLOT(setADCData(ADCData)));

//    QObject::connect(&hoarder, SIGNAL(sendADCData(ADCData)), &analyzer, SLOT(setADCData(ADCData)));
//    QObject::connect(&analyzer, SIGNAL(Inhalations(QVector<exhal>,ADCData)), &calc, SLOT(setIngs(QVector<exhal>,ADCData)));
//    QObject::connect(&calc, SIGNAL(signalParameters(parameters,ADCData)), &calib, SLOT(signalAndParams(parameters,ADCData)));

//    adc.startADC(10000);

//    FILE *in = fopen("in.csv", "r");
//    FILE *out = fopen("out.csv", "w");
//    int d = 0;
//    QVector<int> vec;
//    for(int i = 0; i < 4480; i++){
//        fscanf(in, "%d\n", &d);
//        vec.push_back(d);
//    }
//    ADCData acqdata;
//    acqdata.data[0] = vec;
//    acqdata.data[1] = vec;
//    acqdata.data[2] = vec;
//    hoarder.setADCData(acqdata);

//    auto res = analyzer.getClear();
//    for(auto & m:res)
//        fprintf(out, "%f\n", m);
//    fclose(in);
//    fclose(out);

    return a.exec();
}
