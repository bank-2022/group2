#include "idlewindow.h"
#include "ui_idlewindow.h"

IdleWindow::IdleWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IdleWindow){
    ui->setupUi(this);
    this->setFixedSize(800,600);
    rfid = NULL;
    tries = 3;

    serialPort = new DLLSerialPort(1);
    pinCodeDLL = new PinCodeDLL();
    dllRestApi = new DLLRestAPI();

    connect(pinCodeDLL, SIGNAL(triesToDLL(int)), this, SLOT(Tries(int)));
    connect(pinCodeDLL, SIGNAL(pinToExe(QString)), this, SLOT(PinSlot(QString)));
    connect(dllRestApi, SIGNAL(SendTriesToExe(int)), this, SLOT(GetTries(int)));
    connect(this, SIGNAL(SendTries(int)), pinCodeDLL, SLOT(getTriesFromEXE(int)));
    connect(this, SIGNAL(sendCloseSignal()), pinCodeDLL, SLOT(closeSignalSlot()));
    connect(dllRestApi, SIGNAL(StatusToExe(QString)), this, SLOT(GetLoginStatus(QString)));

    connect(this, SIGNAL(sendAuthInfo(QString,QString)), dllRestApi, SLOT(LoginSlot(QString,QString)));
    HandleCard();
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
    delete dllRestApi;
    pinCodeDLL = nullptr;
    serialPort = nullptr;
    dllRestApi = nullptr;
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
    qDebug() << "triesamount" << tries;
}

void IdleWindow::GetLoginStatus(QString status)
{
    if(status == "success"){
        qDebug() << "Login successful";
        mainWindow = new MainWindow();

        emit sendCloseSignal();
        this->hide();

        mainWindow->show();
    }
    else{
        qDebug() << "Login not successful";
        tries--;
        emit SendTries(tries);
    }
}

void IdleWindow::HandleCard(){
    //rfid = serialPort->GetRFID(); insert real card here
    rfid = (char*) "254654657328";
    if( rfid != NULL){
        delete serialPort;
        serialPort = nullptr;
        dllRestApi->GetTriesFromApi(rfid);
    }
}

void IdleWindow::on_nappi_clicked()
{
    pinCodeDLL->ShowWindow();
    HandleCard();
}

void IdleWindow::GetTries(int x)
{
    emit SendTries(x);
    qDebug() << "Sent tries to pincodeui";
}
