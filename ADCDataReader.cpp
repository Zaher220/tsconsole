#include "ADCDataReader.h"

ADCDataReader::ADCDataReader(QObject *parent):QObject(parent)
{
    strcpy_s(m_adc_name, "usb3000");
}

ADCDataReader::~ADCDataReader()
{
}

bool ADCDataReader::initADC()
{
    WORD i;

    // проверим версию используемой библиотеки Rtusbapi.dll
    if ((DllVersion = RtGetDllVersion()) != CURRENT_VERSION_RTUSBAPI){
        char String[128];
        sprintf(String, " Rtusbapi.dll Version Error!!!\n   Current: %1u.%1u. Required: %1u.%1u",
                DllVersion >> 0x10, DllVersion & 0xFFFF,
                CURRENT_VERSION_RTUSBAPI >> 0x10, CURRENT_VERSION_RTUSBAPI & 0xFFFF);

        CleanupCrushADCInstance(String);
        return false;
    }
    else
        printf(" Rtusbapi.dll Version --> OK\n");

    // получим указатель на интерфейс модуля USB3000
    char *module_name = m_adc_name;
    pModule = static_cast<IRTUSB3000 *>(RtCreateInstance(module_name));

    if (!pModule){
        CleanupCrushADCInstance(" Module Interface --> Bad\n");
        return false;
    }
    else
        printf(" Module Interface --> OK\n");

    // попробуем обнаружить модуль USB3000 в первых 127 виртуальных слотах
    for (i = 0x0; i < MaxVirtualSoltsQuantity; i++) if (pModule->OpenDevice(i)) break;
    // что-нибудь обнаружили?
    if (i == MaxVirtualSoltsQuantity){
        CleanupCrushADCInstance(" Can't find module USB3000 in first 127 virtual slots!\n");
        return false;
    }
    else
        printf(" OpenDevice(%u) --> OK\n", i);

    // попробуем получить дескриптор (handle) устройства
    ModuleHandle = pModule->GetModuleHandle();
    if (ModuleHandle == INVALID_HANDLE_VALUE){
        CleanupCrushADCInstance(" GetModuleHandle() --> Bad\n");
        return false;
    }
    else printf(" GetModuleHandle() --> OK\n");

    // прочитаем название обнаруженного модуля
    if (!pModule->GetModuleName(ModuleName)) {
        CleanupCrushADCInstance(" GetModuleName() --> Bad\n");
        return false;
    }
    else
        printf(" GetModuleName() --> OK\n");

    // проверим, что это 'USB3000'
    if (strcmp(ModuleName, "USB3000")) {
        CleanupCrushADCInstance(" The module is not 'USB3000'\n");
        return false;
    }
    else
        printf(" The module is 'USB3000'\n");

    // узнаем текущую скорость работы шины USB20
    if (!pModule->GetUsbSpeed(&UsbSpeed)){
        CleanupCrushADCInstance(" GetUsbSpeed() --> Bad\n");
        return false;
    }
    else
        printf(" GetUsbSpeed() --> OK\n");
    // теперь отобразим версию драйвера AVR
    printf(" USB Speed is %s\n", UsbSpeed ? "HIGH (480 Mbit/s)" : "FULL (12 Mbit/s)");

    // прочитаем серийный номер модуля
    if (!pModule->GetModuleSerialNumber(ModuleSerialNumber)){
        CleanupCrushADCInstance(" GetModuleSerialNumber() --> Bad\n");
        return false;
    }
    else
        printf(" GetModuleSerialNumber() --> OK\n");
    // теперь отобразим серийный номер модуля
    printf(" Module Serial Number is %s\n", ModuleSerialNumber);

    // прочитаем версию драйвера AVR
    if (!pModule->GetAvrVersion(AvrVersion)) {
        CleanupCrushADCInstance(" GetAvrVersion() --> Bad\n");
        return false;
    }
    else
        printf(" GetAvrVersion() --> OK\n");
    // теперь отобразим версию драйвера AVR
    printf(" Avr Driver Version is %s\n", AvrVersion);

    // код драйвера DSP возьмём из соответствующего ресурса штатной DLL библиотеки
    if (!pModule->LOAD_DSP()) {
        CleanupCrushADCInstance(" LOAD_DSP() --> Bad\n");
        return false;
    }
    else
        printf(" LOAD_DSP() --> OK\n");

    // проверим загрузку модуля
    if (!pModule->MODULE_TEST()) {
        CleanupCrushADCInstance(" MODULE_TEST() --> Bad\n");
        return false;
    }
    else
        printf(" MODULE_TEST() --> OK\n");

    // получим версию загруженного драйвера DSP
    if (!pModule->GET_DSP_INFO(&di)) {
        CleanupCrushADCInstance(" GET_DSP_VERSION() --> Bad\n");
        return false;
    }
    else
        printf(" GET_DSP_VERSION() --> OK\n");
    // теперь отобразим версию загруженного драйвера DSP
    printf(" DSP Driver version is %1u.%1u\n", di.DspMajor, di.DspMinor);

    // обязательно проинициализируем поле size структуры RTUSB3000::FLASH
    fi.size = sizeof(RTUSB3000::FLASH);
    // получим информацию из ППЗУ модуля
    if (!pModule->GET_FLASH(&fi)) {
        CleanupCrushADCInstance(" GET_MODULE_DESCR() --> Bad\n");
        return false;
    }
    else
        printf(" GET_MODULE_DESCR() --> OK\n");

    // обязательно проинициализируем поле size структуры RTUSB3000::INPUT_PARS
    ip.size = sizeof(RTUSB3000::INPUT_PARS);
    // получим текущие параметры работы АЦП
    if (!pModule->GET_INPUT_PARS(&ip)){
        CleanupCrushADCInstance(" GET_INPUT_PARS() --> Bad\n");
        return false;
    }
    else
        printf(" GET_INPUT_PARS() --> OK\n");

    // установим желаемые параметры АЦП
    ip.CorrectionEnabled = true;				// разрешим корректировку вводимых данных
    ip.InputClockSource = RTUSB3000::INTERNAL_INPUT_CLOCK;	// будем использовать внутренние тактовые испульсы для ввода данных
    //	ip.InputClockSource = RTUSB3000::EXTERNAL_INPUT_CLOCK;	// будем использовать внешние тактовые испульсы для ввода данных
    ip.SynchroType = RTUSB3000::NO_SYNCHRO;	// не будем использовать никакую синхронизацию при вводе данных
    //	ip.SynchroType = RTUSB3000::TTL_START_SYNCHRO;	// будем использовать цифровую синхронизацию старта при вводе данных
    ip.ChannelsQuantity = CHANNEL_QUANTITY;					// четыре активных канала
    for (i = 0x0; i < ip.ChannelsQuantity; i++)
        ip.ControlTable[i] = (WORD)(i);
    //меньше 550 Гц ацп не умеет переключать каналы.
    //Поэтому берём 3кГц - на канал получается 1кГц, а больше методом усреднения по десять получаем частоту измерения в 100 Гц
    ip.InputRate = 3.0;//1.0;//ReadRate;					// частота работы АЦП в кГц

    //ip.ChannelRate = 0.01;
    ip.InterKadrDelay = 0.0;					// межкадровая задержка - пока всегда устанавливать в 0.0
    ip.InputFifoBaseAddress = 0x0;  			// базовый адрес FIFO буфера АЦП
    ip.InputFifoLength = 0x3000;	 			// длина FIFO буфера АЦП
    ip.InputFifoLength = 0x400;
    // будем использовать фирменные калибровочные коэффициенты, которые храняться в ППЗУ модуля
    for (i = 0x0; i < 8; i++) {
        ip.AdcOffsetCoef[i] = fi.AdcOffsetCoef[i];
        ip.AdcScaleCoef[i] = fi.AdcScaleCoef[i];
    }
    // передадим требуемые параметры работы АЦП в модуль
    if (!pModule->SET_INPUT_PARS(&ip)){
        CleanupCrushADCInstance(" SET_INPUT_PARS() --> Bad\n");
        return false;
    }
    else
        printf(" SET_INPUT_PARS() --> OK\n");

    // отобразим на экране дисплея параметры работы модуля USB3000
    printf(" \n");
    printf(" Module USB3000 (S/N %s) is ready ... \n", ModuleSerialNumber);
    printf(" Adc parameters:\n");
    printf("   InputClockSource is %s\n", ip.InputClockSource ? "EXTERNAL" : "INTERNAL");
    printf("   SynchroType is %s\n", ip.SynchroType ? "TTL_START_SYNCHRO" : "NO_SYNCHRO");
    printf("   ChannelsQuantity = %2d\n", ip.ChannelsQuantity);
    printf("   AdcRate = %8.3f kHz\n", ip.InputRate);
    printf("   InterKadrDelay = %2.4f ms\n", ip.InterKadrDelay);
    printf("   ChannelRate = %8.3f kHz\n", ip.ChannelRate);
    return true;
}

bool ADCDataReader::WaitingForRequestCompleted(OVERLAPPED *ReadOv, LPDWORD byte_N)
{
    while (true)
    {
        if ( !is_acq_started )
            return false;
        if (GetOverlappedResult(ModuleHandle, ReadOv, byte_N, FALSE)){
            break;
        }
        else
            if (GetLastError() != ERROR_IO_INCOMPLETE) {
                ThreadErrorNumber = 0x3;
                return false;
            }
            else{
                Sleep(20);
            }
    }
    return true;
}

int ADCDataReader::getSamples_number() const
{
    return m_samples_number;
}

void ADCDataReader::setSamples_number(int samples_number)
{
    m_samples_number = samples_number;
}


//------------------------------------------------------------------------
// Отобразим сообщение с ошибкой
//------------------------------------------------------------------------
void ADCDataReader::ShowThreadErrorMessage(void)
{
    switch (ThreadErrorNumber)
    {
    case 0x0:
        break;
    case 0x1:
        // если программа была злобно прервана, предъявим ноту протеста
        printf("\n READ Thread: The program was terminated! :(((\n");
        break;
    case 0x2:
        printf("\n READ Thread: ReadData() --> Bad :(((\n");
        break;
    case 0x3:
        printf("\n READ Thread: Read Request --> Bad :(((\n");
        //			printf("\n READ Thread: Timeout is occured :(((\n");
        break;
    case 0x4:
        printf("\n READ Thread: Buffer Data Error! :(((\n");
        break;
    case 0x5:
        printf("\n READ Thread: START_READ() --> Bad :(((\n");
        break;
    case 0x6:
        printf("\n READ Thread: STOP_READ() --> Bad! :(((\n");
        break;
    case 0x7:
        printf("\n READ Thread: Can't complete input and output (I/O) operations! :(((");
        break;
    default:
        printf("\n READ Thread: Unknown error! :(((\n");
        break;
    }
    return;
}

//------------------------------------------------------------------------
// вывод сообщения и, если нужно, аварийный выход из программы
//------------------------------------------------------------------------
void ADCDataReader::CleanupCrushADCInstance(QString ErrorString)
{
    // подчищаем интерфейс модуля
    if (pModule)
    {
        // освободим интерфейс модуля
        if (!pModule->ReleaseInstance())
            printf(" ReleaseInstance() --> Bad\n");
        else
            printf(" ReleaseInstance() --> OK\n");
        // обнулим указатель на интерфейс модуля
        pModule = NULL;
    }
    // освободим идентификатор потока сбора данных
    if (hReadThread) {
        CloseHandle(hReadThread);
        hReadThread = NULL;
    }

    // выводим текст сообщения
    if ( ErrorString.length() > 0 )
        printf("%s",ErrorString.toStdString().c_str());

    else return;
}

void ADCDataReader::startADC(int samples_number)
{
    is_acq_started = true;
    setSamples_number(samples_number);
    initADC();
    // сбросим флаг ошибок потока ввода данных
    ThreadErrorNumber = 0x0;

    connect(this, &ADCDataReader::started, this, &ADCDataReader::processADC);
    emit started();
}

void ADCDataReader::stopADC()
{
    is_acq_started = false;

    // подчищаем интерфейс модуля
    if (pModule != NULL){
        // освободим интерфейс модуля
        if (!pModule->ReleaseInstance())
            printf(" ReleaseInstance() --> Bad\n");
        else
            printf(" ReleaseInstance() --> OK\n");
        pModule = NULL;
    }
}

void ADCDataReader::processADC()
{
    WORD i;
    // номер запроса на сбор данных
    WORD RequestNumber;
    // идентификатор массива их двух событий
    HANDLE ReadEvent[2];
    // массив OVERLAPPED структур из двух элементов
    OVERLAPPED ReadOv[2];

    DWORD BytesTransferred[2];

    // остановим ввод данных и одновременно прочистим соответствующий канал bulk USB
    if( pModule == NULL || pModule == nullptr ){
        emit finished();
        return;
    }
    if (!pModule->STOP_READ()) {
        ThreadErrorNumber = 0x6;
        IsThreadComplete = true;
        emit finished();
        return;
    }

    // создадим два события
    ReadEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
    memset(&ReadOv[0], 0, sizeof(OVERLAPPED)); ReadOv[0].hEvent = ReadEvent[0];
    ReadEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    memset(&ReadOv[1], 0, sizeof(OVERLAPPED)); ReadOv[1].hEvent = ReadEvent[1];

    // делаем предварительный запрос на ввод данных
    RequestNumber = 0x0;
    if (!pModule->ReadData(ReadBuffer, &DataStep, &BytesTransferred[RequestNumber], &ReadOv[RequestNumber]))
        if (GetLastError() != ERROR_IO_PENDING) {
            CloseHandle(ReadEvent[0]);
            CloseHandle(ReadEvent[1]);
            ThreadErrorNumber = 0x2;
            IsThreadComplete = true;
            emit finished();
            return;
        }

    // теперь запускаем ввод данных

    if (pModule->START_READ())
    {
        // цикл сбора данных
        i = 0;
        m_samples_count = 0;
        ADCData data;

        while (is_acq_started && (m_samples_number == -1 || m_samples_count <= m_samples_number) ){
            RequestNumber ^= 0x1;
            // сделаем запрос на очередную порции данных
            if (!pModule->ReadData(ReadBuffer /*!!!+ i*DataStep*/, &DataStep, &BytesTransferred[RequestNumber], &ReadOv[RequestNumber]))
                if (GetLastError() != ERROR_IO_PENDING) {
                    ThreadErrorNumber = 0x2;
                    break;
                }
            // ждём окончания операции сбора очередной порции данных
            if (!WaitingForRequestCompleted(&ReadOv[RequestNumber ^ 0x1], &BytesTransferred[RequestNumber])){
                break;
            }

            qDebug()<<"RequestNumber"<<RequestNumber;
            qDebug()<<"BytesTransferred"<< BytesTransferred[RequestNumber]<<DataStep;

            if(i >= 1){
                for (int k = 0; k < DataStep/*!!!BytesTransferred[RequestNumber]*/; k += CHANNEL_QUANTITY){ //FIXME нужно получать 1 точку из 10 усреднением
                    data.data[0].append(ReadBuffer[k]);
                    data.data[1].append(ReadBuffer[k+1]);
                    data.data[2].append(ReadBuffer[k+2]);
                }
                m_samples_count += data.data[0].size();
                memset(ReadBuffer, 0, /*!!!NBlockRead **/ DataStep );
                emit newData(data);
                data.clear();
            }
            i++;
        }
        // ждём окончания операции сбора последней порции данных
        if (!ThreadErrorNumber){
            RequestNumber ^= 0x1;
            WaitingForRequestCompleted(&ReadOv[RequestNumber ^ 0x1], &BytesTransferred[RequestNumber]);
        }
    }
    else {
        ThreadErrorNumber = 0x5;
    }

    if( pModule == NULL || pModule == nullptr ){
        emit finished();
        return;
    }
    // остановим ввод данных
    if (!pModule->STOP_READ())
        ThreadErrorNumber = 0x6;
    // если надо, то прервём незавершённый асинхронный запрос
    if (!CancelIo(pModule->GetModuleHandle()))
        ThreadErrorNumber = 0x7;
    // освободим все идентификаторы событий
    for (i = 0x0; i < 0x2; i++)
        CloseHandle(ReadEvent[i]);
    // небольшая задержка
    Sleep(100);
    // установим флажок окончания потока сбора данных
    IsThreadComplete = true;
    // теперь можно воходить из потока сбора данных

    emit finished();
    return;
}

bool ADCDataReader::isReady()
{
    return true;
    if( initADC()){
        CleanupCrushADCInstance("On test init");
        return true;
    }else
        return false;
}

QVector<int> ADCDataReader::getSamplesSinc(int channel, int samplesNumber)
{
    is_acq_started = true;

    stopADC();

    if ( !initADC())
        return QVector<int>();

    // остановим ввод данных и одновременно прочистим соответствующий канал bulk USB
    if (!pModule->STOP_READ()) {
        return QVector<int>();
    }

    short AdcBuffer[3];
    int avgs[3];
    QVector<int> vec[3];
    if (pModule->START_READ())
    {
        for(int i = 0; i < samplesNumber; i++)
        {
            if (pModule->READ_KADR(AdcBuffer)) {
                avgs[0] = ((avgs[0]*i)+AdcBuffer[0])/(i+1);
                avgs[1] = ((avgs[1]*i)+AdcBuffer[1])/(i+1);
                avgs[2] = ((avgs[2]*i)+AdcBuffer[2])/(i+1);
                vec[0].push_back(AdcBuffer[0]);
                vec[1].push_back(AdcBuffer[1]);
                vec[2].push_back(AdcBuffer[2]);
            }
        }
    }

    // остановим ввод данных
    if (!pModule->STOP_READ())
        return QVector<int>();

    // подчищаем интерфейс модуля
    if (pModule != NULL){
        // освободим интерфейс модуля
        if (!pModule->ReleaseInstance())
            printf(" ReleaseInstance() --> Bad\n");
        else
            printf(" ReleaseInstance() --> OK\n");
        // обнулим указатель на интерфейс модуля
        pModule = NULL;
    }

    is_acq_started = false;
    if( channel > 3 )
        return vec[channel];
    return QVector<int>();
}
