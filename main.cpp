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
    //    ioton.beginResearch(50000);
    //    qDebug()<<"before sleep";
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



    FILE *in = fopen("raw_data_3.csv", "r");
    FILE *out = fopen("out.csv", "w");
    int d1,d2,d3;
    QVector<int> vec1,vec2,vec3;
    for(int i = 0; i < 50176; i++){
        fscanf_s(in, "%d;%d;%d;\n", &d1, &d2, &d3);
        vec1.push_back(d1);
        vec2.push_back(d2);
        vec3.push_back(d3);

        //        if( i%100 == 0 ){
        //            ADCData acqdata;
        //            acqdata.data[0] = vec1;
        //            acqdata.data[1] = vec2;
        //            acqdata.data[2] = vec3;
        //            vec1.clear();
        //            vec2.clear();
        //            vec3.clear();
        //            ioton.appendData(acqdata);
        //        }

    }
    ADCData acqdata;
    acqdata.data[0] = vec1;
    acqdata.data[1] = vec2;
    acqdata.data[2] = vec3;
    vec1.clear();
    vec2.clear();
    vec3.clear();
    ioton.appendData(acqdata);
    //    auto res = analyzer.getClear();
    //    for(auto & m:res)
    //        fprintf(out, "%f\n", m);
    fclose(in);
    fclose(out);

    return a.exec();
}
