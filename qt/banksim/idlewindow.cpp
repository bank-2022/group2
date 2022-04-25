#include "idlewindow.h"
#include "ui_idlewindow.h"

IdleWindow::IdleWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IdleWindow){
    ui->setupUi(this);
    this->setFixedSize(800,600);
    rfid = "";
    tries = 3;
    this->show();
    mainWindow = new MainWindow();
    mainWindow->hide();
    pinCodeDLL = new PinCodeDLL();
    dllRestApi = mainWindow->api;

    connect(pinCodeDLL, SIGNAL(triesToDLL(int)), this, SLOT(Tries(int)));
    connect(pinCodeDLL, SIGNAL(pinToExe(QString)), this, SLOT(PinSlot(QString)));
    connect(dllRestApi, SIGNAL(SendTriesToExe(int)), this, SLOT(GetTries(int)));
    connect(this, SIGNAL(SendTries(int)), pinCodeDLL, SLOT(getTriesFromEXE(int)));
    connect(this, SIGNAL(sendCloseSignal()), pinCodeDLL, SLOT(closeSignalSlot()));
    connect(dllRestApi, SIGNAL(StatusToExe(QString)), this, SLOT(GetLoginStatus(QString)));
    connect(mainWindow, SIGNAL(logOutSignal()), this, SLOT(LogOutSlot()));
    connect(this, SIGNAL(sendAuthInfo(QString,QString)), dllRestApi, SLOT(LoginSlot(QString,QString)));
    //HandleCard();
}
IdleWindow::~IdleWindow(){
    disconnect(pinCodeDLL, SIGNAL(triesToDLL(int)), this, SLOT(Tries(int)));
    disconnect(pinCodeDLL, SIGNAL(pinToExe(QString)), this, SLOT(PinSlot(QString)));
    disconnect(this, SIGNAL(SendTries(int)), pinCodeDLL, SLOT(getTriesFromEXE(int)));
    disconnect(this, SIGNAL(sendCloseSignal()), pinCodeDLL, SLOT(closeSignalSlot()));

    disconnect(this, SIGNAL(sendAuthInfo(QString,QString)), dllRestApi, SLOT(LoginSlot(QString,QString)));

    delete ui;
    delete pinCodeDLL;
    delete serialPort;
    pinCodeDLL = nullptr;
    serialPort = nullptr;
    if(mainWindow != nullptr){
        delete mainWindow;
        mainWindow = nullptr;
    }
}

void IdleWindow::PinSlot(QString pin){
    pinCode = pin;
    qDebug() << "pinReceived from pincodedll" << pin;

    emit sendAuthInfo(QString(rfid),pin);
}

void IdleWindow::Tries(int triesAmount){
    tries = triesAmount;
}

void IdleWindow::GetLoginStatus(QString status)
{
    if(status == "success"){
        qDebug() << "Login successful";

        emit sendCloseSignal();
        this->hide();
        rfid = "";
        mainWindow->show();
    }
    else{
        qDebug() << "Login not successful";
    }
}

void IdleWindow::HandleCard(){
<<<<<<< HEAD
    //rfid = serialPort->GetRFID(); insert real card here
    rfid = (char*) "254654657328";
    if( rfid != NULL){
=======
      serialPort = new DLLSerialPort(4);
//    rfid = serialPort->GetRFID(); //insert real card here
//    qDebug()<<rfid<<" debuggg";
    rfid = "993795686002";
  
    if( rfid != ""){
>>>>>>> 173bd9ae183edadaa02b6e4e9ca08a3caa5eea90
        delete serialPort;
        serialPort = nullptr;
        dllRestApi->GetTriesFromApi(rfid);
    }
}

void IdleWindow::on_nappi_clicked()
{
    HandleCard();
}

void IdleWindow::GetTries(int x)
{
    emit SendTries(x);
    qDebug() << "Sent tries to pincodeui";
    if(!pinCodeDLL->isVisible()){
        pinCodeDLL->ShowWindow();
    }
}

void IdleWindow::LogOutSlot()
{
    this->show();
    dllRestApi = mainWindow->api;
}
