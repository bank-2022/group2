#ifndef DLLSERIALPORT_H
#define DLLSERIALPORT_H
#include "dllserialportengine.h"
#include "DLLSerialPort_global.h"
#include <QString>
class DLLSERIALPORT_EXPORT DLLSerialPort
{
public:
    DLLSerialPort(int comPort);
    ~DLLSerialPort();
    QString GetRFID();

private:
     DLLSerialPortEngine* engineClass;
     QString rfid;
};

#endif // DLLSERIALPORT_H
