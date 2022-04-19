#include "pincodeui.h"
#include "ui_pincodeui.h"

PinCodeUI::PinCodeUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PinCodeUI)
{
    ui->setupUi(this);

    // append all buttons 0-9 to button vector
    buttons.append(ui->btn_1);
    buttons.append(ui->btn_2);
    buttons.append(ui->btn_3);
    buttons.append(ui->btn_4);
    buttons.append(ui->btn_5);
    buttons.append(ui->btn_6);
    buttons.append(ui->btn_7);
    buttons.append(ui->btn_8);
    buttons.append(ui->btn_9);
    buttons.append(ui->btn_0);

    // connect all buttons on the vector to handleClick slot
    foreach(QPushButton *btn, buttons){
        connect(btn, SIGNAL(clicked()), this, SLOT(handleClick()));
    }

    // create timer and start it
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(autoTimeout()));
}

PinCodeUI::~PinCodeUI()
{
    qDebug() << "Pincodeui destroyed";
    disconnect(timer, SIGNAL(timeout()), this, SLOT(autoTimeout()));

    foreach(QPushButton *btn, buttons){
        disconnect(btn, SIGNAL(clicked()), this, SLOT(handleClick()));
    }

    delete ui;
    delete timer;
    timer = nullptr;
}

void PinCodeUI::setPinCodeText(QString text)
{
    // set pinCodeTextBox text to QString
    ui->pinCodeTextBox->setText(text);
}

void PinCodeUI::clearPin()
{
    // clear pinCode and clear pinCodeTextBox
    pinCode = QString("");
    censoredPinCode = "";

    setPinCodeText(censoredPinCode);
    ui->btn_ok->setEnabled(false);
}

void PinCodeUI::setInfoText(QString txt)
{
    ui->infoTextBox->setText(txt);
}

void PinCodeUI::resetTimer()
{
    timer->stop();
    timer->start(10000);
}

void PinCodeUI::handleClick()
{

    QObject * sender = QObject::sender();

    QString btnName = sender->objectName();

    // check if pinCode length is 4 or more then return out

    if (pinCode.length() >= 4){
        return;
    }

    // loop through all the buttons in buttons vector assigned in constructor
    // check if current button's name matches the one in the vector
    // assign pushed buttons value to the pin code
    foreach (QPushButton *btn, buttons){
        if(btn->objectName() == btnName){
            qDebug() << "Pushed" << btn->text() << "button";
            pinCode += btn->text();

            // clear censored pincode
            censoredPinCode = "";

            // append '*' to censored pincode based on pinCodes length
            for (int i = 0; i < pinCode.length(); i++){
                censoredPinCode += "*";
            }

            // update text box
            setPinCodeText(censoredPinCode);
        }
    }

    if (pinCode.length() >= 4 && currentTries > 0){
        ui->btn_ok->setEnabled(true);
    }

    resetTimer();
}

void PinCodeUI::on_btn_clear_clicked()
{
    clearPin();
    qDebug() << "Pin code cleared";

    resetTimer();
}

void PinCodeUI::on_btn_ok_clicked()
{
    // check if pinCode length is less than 4 then return out
    if (pinCode.length() < 4){
        return;
    }

    qDebug() << "Tries" << currentTries;

    if (currentTries <= 0){
        return;
    }

    qDebug() << "Pin code:" << pinCode;
    qDebug() << "Emitting signal to Pin code DLL";

    emit sendPinCode(pinCode);

    clearPin();
    ui->btn_ok->setEnabled(false);
}

void PinCodeUI::autoTimeout()
{
    if(this->isVisible()){
        qDebug() << "Auto timeout because no action was taken in 10 s";

        clearPin();

        this->close();
    }
}

void PinCodeUI::getTries(int tries)
{
    timer->start(10000);

    qDebug() << "Got tries from exe";
    qDebug() << tries;
    currentTries = tries;

    if (currentTries <= 0){
        ui->infoTextBox->setText("No attempts left");
        ui->btn_ok->setEnabled(false);

    }else{
        ui->infoTextBox->setText(QString::number(currentTries) + " attempts left");
    }
}

void PinCodeUI::on_btn_cancel_clicked()
{
    clearPin();

    this->close();
}

