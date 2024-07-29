#ifndef PORTWIDGETS_H
#define PORTWIDGETS_H

#include <QWidget>
#include <QSerialPort>
#include <unordered_map>

namespace Ui {
class PortWidgets;
}

class PortWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit PortWidgets(QWidget *parent = nullptr);
    ~PortWidgets();
signals:
    void receivedata(QByteArray data);

private slots:
    void on_ptn_refresh_clicked();
    void receive_data();

    void on_ptn_conn_clicked();

public:
    void write_data(QByteArray writedata);
private:
    Ui::PortWidgets *ui;
    QSerialPort *myPort;//串口指针

    std::unordered_map<int, QSerialPort::BaudRate> baudRateMap =//波特率
        {
            { 0, QSerialPort::Baud1200 },{ 1, QSerialPort::Baud2400 },{ 2, QSerialPort::Baud4800 },
            { 3, QSerialPort::Baud9600 },{ 4, QSerialPort::Baud19200 },{ 5, QSerialPort::Baud38400 },
            { 6, QSerialPort::Baud57600 },{ 7, QSerialPort::Baud115200 }
        };
    std::unordered_map<int, QSerialPort::DataBits>dataMap =//数据位
        {
            { 0,QSerialPort::Data5 },{ 1,QSerialPort::Data6 },{ 2,QSerialPort::Data7 },{ 3,QSerialPort::Data8 }
        };
    std::unordered_map<int, QSerialPort::Parity>parityMap =//校验位
        {
            { 0,QSerialPort::NoParity },{ 1,QSerialPort::OddParity },{ 2,QSerialPort::EvenParity }
        };
    std::unordered_map<int, QSerialPort::StopBits> stopMap = //停止位
        {{0, QSerialPort::OneStop}, {1, QSerialPort::OneAndHalfStop}, {2, QSerialPort::TwoStop}};
};

#endif // PORTWIDGETS_H
