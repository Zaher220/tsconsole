#include "calibrator.h"

Calibrator::Calibrator(QObject *parent) : QObject(parent)
{

}

void Calibrator::signalAndParams(parameters params, ADCData data)
{
    params.debug();
}
