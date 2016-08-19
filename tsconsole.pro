QT += core
QT -= gui

CONFIG += c++11


TARGET = tsconsole
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ADCDataReader.cpp \
    datahoarder.cpp \
    analyze/signalanalyzer.cpp \
    analyze/volumevaluescalc.cpp \
    calibrator.cpp \
    temperatureparameerscalulator.cpp

HEADERS += \
    ADCDataReader.h \
    datahoarder.h \
    datatypes.h \
    analyze/signalanalyzer.h \
    analyze/volumevaluescalc.h \
    calibrator.h \
    temperatureparameerscalulator.h

win32: LIBS += -L$$PWD/lib/ -lRtusbapi

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

