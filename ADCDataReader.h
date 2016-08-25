#ifndef ADCDATAREADER_H
#define ADCDATAREADER_H
#include <QObject>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "Rtusbapi.h"
#include "datatypes.h"
#include <QDebug>

class ADCDataReader: public QObject
{
    Q_OBJECT
public:
    explicit ADCDataReader(QObject *parent = 0);
    ~ADCDataReader();
    int getSamples_number() const;
    void setSamples_number(int samples_number);
    bool isReady();
    QVector<int> getSamplesSinc(int channel, int samplesNumber);
public slots:
    void startADC(int samples_number);
    void stopADC();
    void processADC();
signals:
    void finished();
    void newData( ADCData );
    void done();
    void changeProgress(int);
    void started();
private:
    bool initADC();
    void ShowThreadErrorMessage(void);
    void CleanupCrushADCInstance(QString ErrorString);
    bool WaitingForRequestCompleted(OVERLAPPED *ReadOv, LPDWORD byte_N);
    bool is_acq_started = false;

    // идентификатор потока ввода
    HANDLE 	hReadThread = 0;
    DWORD 	ReadTid;

    // текущая версия библиотеки Rtusbapi.dll
    DWORD DllVersion;
    // указатель на интерфейс модуля
    IRTUSB3000 *pModule = NULL;
    // хэндл модуля
    HANDLE ModuleHandle;
    // название модуля
    char ModuleName[10];
    // скорость работы шины USB
    BYTE UsbSpeed;
    // серийный номер модуля
    char ModuleSerialNumber[9];
    // версия драйвера AVR
    char AvrVersion[5];
    // структура, содержащая информацию о версии драйвера DSP
    RTUSB3000::DSP_INFO di;
    // структура информации в ППЗУ модуля
    RTUSB3000::FLASH fi;
    // структура параметров работы АЦП
    RTUSB3000::INPUT_PARS ip;

    // максимально возможное кол-во опрашиваемых виртуальных слотов
    const WORD MaxVirtualSoltsQuantity = 4;
    // частота  ввода данных
    const double ReadRate = 0.512;
    //max возможное кол-во передаваемых отсчетов (кратное 32) для ф. ReadData и WriteData()
    const DWORD CHANNEL_QUANTITY = 3;
    DWORD DataStep = 512 * CHANNEL_QUANTITY;
    SHORT	ReadBuffer[512*3];//FIXME DEFINE ???

    // номер ошибки при выполнении потока сбора данных
    WORD ThreadErrorNumber;
    // флажок завершения потоков ввода данных
    bool IsThreadComplete = false;

    HANDLE hMutex ;
    qint32 m_samples_number = -1;//1800000;//FIXME похоже попытка мерить время АЦП - это не верно
    qint32 m_samples_count = 0;
    char m_adc_name[8] ;
};

#endif
