#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <dllrestapi.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    DLLRestAPI *api;
private slots:

    void on_nosto_clicked();
    void on_takaisin_clicked();
    void on_talletus_clicked();
    void on_takaisin_talletus_clicked();
    void on_amount_clicked();

    void on_clear_clicked();

    void on_saldo_nappi_clicked();

    void on_close_button_clicked();

    void on_close_clicked();

    void on_tilitapahtumat_clicked();

    void updateLogsView();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
