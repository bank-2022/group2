#include "dllserialport.h"
QString rfid1;
void ReadCard(char* data);

DLLSerialPort::DLLSerialPort(int comPort)
{
    engineClass = new DLLSerialPortEngine(comPort);
    engineClass->OpenPort(9600, NOPARITY, 8, ONESTOPBIT);
}

DLLSerialPort::~DLLSerialPort()
{
    engineClass->ClosePort();
    delete engineClass;
    engineClass = nullptr;
}

QString DLLSerialPort::GetRFID()
{
    engineClass->ReadCard(18, ReadCard);
    engineClass->ClosePort();
    return rfid1;
}

void ReadCard(char* data)
{
    cout << "rfid: in read card " << data << endl;
    rfid1 = data;
}
