#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QMainWindow>
#include "PortWidgets.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SerialPort; }
QT_END_NAMESPACE

class SerialPort : public QMainWindow
{
    Q_OBJECT

public:
    SerialPort(QWidget *parent = nullptr);
    ~SerialPort();

private slots:
    void on_ptn_conn_clicked();

    void on_ptn_clear_clicked();//清除接收框的数据
    void receive_data(QByteArray data);//接收串口数据的槽
    void on_lineEdit_dataSend_returnPressed();

signals:
    void writedata(QByteArray data);

private:
    Ui::SerialPort *ui;
    PortWidgets* m_portWidget;
};
#endif // SERIALPORT_H
