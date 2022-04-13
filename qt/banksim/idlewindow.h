#ifndef IDLEWINDOW_H
#define IDLEWINDOW_H
#include "dllserialport.h"
#include "pincodedll.h"
#include "mainwindow.h"
#include "dllrestapi.h"

#include <QMainWindow>
#include <QString>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class IdleWindow; }
QT_END_NAMESPACE

class IdleWindow : public QMainWindow
{
    Q_OBJECT

public:
    IdleWindow(QWidget *parent = nullptr);
    ~IdleWindow();


public slots:
    void PinSlot(QString);
    void Tries(int);
    void GetLoginStatus(QString);

signals:
    void SendTries(int);
    void sendCloseSignal();
    void sendAuthInfo(QString, QString);
private slots:
    void on_nappi_clicked();
    void GetTries(int);

private:
    Ui::IdleWindow *ui;
    DLLSerialPort *serialPort;
    PinCodeDLL *pinCodeDLL;
    MainWindow *mainWindow = nullptr;
    void HandleCard();
    char* rfid;
    QString pinCode;
    int tries;
    DLLRestAPI *dllRestApi;
};
#endif // IDLEWINDOW_H
