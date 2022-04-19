#ifndef PINCODEUI_H
#define PINCODEUI_H

#include <QtWidgets/QDialog>
#include <QDebug>
#include <QTimer>

namespace Ui {
class PinCodeUI;
}

class PinCodeUI : public QDialog
{
    Q_OBJECT

public:
    explicit PinCodeUI(QWidget *parent = nullptr);
    ~PinCodeUI();

private:
    Ui::PinCodeUI *ui;
    QString pinCode;
    QVector <QPushButton *> buttons;
    QString censoredPinCode;

    void setPinCodeText(QString);
    void clearPin();

    void setInfoText(QString);

    void resetTimer();

    int currentTries = 1;

    QTimer * timer;

private slots:

    void handleClick();

    void on_btn_clear_clicked();
    void on_btn_ok_clicked();

    void autoTimeout();

    void on_btn_cancel_clicked();

public slots:
    void getTries(int);
signals:
    void sendPinCode(QString);
};

#endif // PINCODEUI_H
