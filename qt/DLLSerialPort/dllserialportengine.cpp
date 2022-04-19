#include "dllserialportengine.h"

DLLSerialPortEngine::DLLSerialPortEngine(int comPortA)
{
    comPort = comPortA;
    hSerial = INVALID_HANDLE_VALUE;
    reading = false;
}

bool DLLSerialPortEngine::OpenPort(int baud, int parity, int dataBits, int stopBits)
{
    ClosePort();

    hSerial = GetSerialPortHandle(comPort);

    if(hSerial == INVALID_HANDLE_VALUE){
        return false;
    }
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    dcbSerialParams.BaudRate = baud;
    dcbSerialParams.ByteSize = dataBits;
    dcbSerialParams.StopBits = stopBits;
    dcbSerialParams.Parity = parity;

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = SERIAL_READ_INTERVAL_TIMEOUT;
    timeouts.ReadTotalTimeoutConstant = SERIAL_READ_TOTAL_TIMEOUT_CONSTANT;
    timeouts.ReadTotalTimeoutMultiplier = SERIAL_READ_TOTAL_TIMEOUT_MULTIPLIER;

    return SetCommState(hSerial, &dcbSerialParams)
            && SetCommTimeouts(hSerial, &timeouts)
            && SetCommMask(hSerial, EV_RXCHAR);
}

void DLLSerialPortEngine::ClosePort()
{
    if(hSerial != INVALID_HANDLE_VALUE)
    {
        reading = false;
        CloseHandle(hSerial);
    }
}

void DLLSerialPortEngine::ReadCard(size_t bufferSize, void (*OnRead)(char *))
{
    char* serialBuffer = (char*)calloc(bufferSize, sizeof (char));

    HANDLE hReadEvent = CreateEvent(NULL, TRUE, FALSE, L"RxEvent");
    OVERLAPPED ovRead = {0};
    ovRead.hEvent = hReadEvent;

    DWORD dwBytesRead;

    reading = true;
    do{
        if(HasOverlappedIoCompleted(&ovRead)){
            if(!ReadFile(hSerial, serialBuffer, bufferSize, &dwBytesRead, &ovRead)){
                DWORD dwError = GetLastError();
                if(dwError != ERROR_IO_PENDING){
                    cout << "An error occurred "; cout << dwError;
                    break;
                }
            }
        }

        if(serialBuffer[0] == 0){
            continue;
        }

        if(GetOverlappedResult(hSerial, &ovRead, &dwBytesRead, FALSE)){
            serialBuffer[0] = '0';
            serialBuffer[1] = '0';
            serialBuffer[2] = '0';
            serialBuffer[15] = '0';
            serialBuffer[14] = '0';
            serialBuffer[13] = '0';

            CloseHandle(hReadEvent);
            reading = false;
        }
    }while(reading);

    OnRead(serialBuffer);
    memset(serialBuffer, 0, bufferSize);
    free(serialBuffer);
    ClosePort();
}

HANDLE DLLSerialPortEngine::GetSerialPortHandle(int comPort)
{
    wchar_t commPortName[5];
    swprintf(commPortName, L"COM%d", comPort);

    HANDLE hComm;
    hComm = CreateFile(commPortName,
                       GENERIC_READ,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                       NULL);
    return hComm;
}

