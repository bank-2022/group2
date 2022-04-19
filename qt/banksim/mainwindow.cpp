#include "mainwindow.h"
#include "ui_mainwindow.h"

double nostoValue = 0.0;

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    this->setFixedSize(800,600);
    api = new DLLRestAPI;

    ui->nostoArvo->setText(QString::number(nostoValue));
    QPushButton *numButtons[6];
    for(int i = 0; i < 6; ++i){
        QString butName = "nosto" + QString::number(i);
        numButtons[i] = MainWindow::findChild<QPushButton *>(butName);
        connect(numButtons[i], SIGNAL(released()), this,
                SLOT(on_amount_clicked()));
    }
    connect(api, SIGNAL(logsUpdatedSignal()), this, SLOT(updateLogsView()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete api;
    api = nullptr;
}
//---------------- NOSTO PAGE-----------------
void MainWindow::on_amount_clicked(){
    QPushButton *button = (QPushButton *)sender();
    nostoValue=button->text().toDouble();
    ui->nostoArvo->setText(QString::number(nostoValue));
}
void MainWindow::on_clear_clicked()
{
    nostoValue = 0.0;
    ui->nostoArvo->setText(QString::number(nostoValue));
}
//
void MainWindow::on_nosto_clicked(){
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_takaisin_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_talletus_clicked(){
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_takaisin_talletus_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_saldo_nappi_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_close_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_close_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_tilitapahtumat_clicked()
{
    api->UpdateLogs(1);
    ui->stackedWidget->setCurrentIndex(1);
}

//---------------------TILITAPAHTUMAT PAGE-----------------------
void MainWindow::updateLogsView()
{
    //ui->listView
}

