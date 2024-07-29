#include "PortWidgets.h"
#include "qserialportinfo.h"
#include "ui_PortWidgets.h"
#include <QString>
#include <QStringList>
#include <qDebug>
PortWidgets::PortWidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PortWidgets)
{
    ui->setupUi(this);
    this->setWindowTitle("PortWidgets");
    this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QStringList baudratelist = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    ui->CmbBaud->addItems(baudratelist);
    ui->CmbBaud->setCurrentIndex(3);

    QStringList datalist = {QString::fromUtf8("5"),
                            QString::fromUtf8("6"),
                            QString::fromUtf8("7"),
                            QString::fromUtf8("8")};
    ui->CmbDataBits->addItems(datalist);
    ui->CmbDataBits->setCurrentIndex(3);

    QStringList checklist = {QString::fromLocal8Bit("No"),
                             QString::fromLocal8Bit("Even"),
                             QString::fromLocal8Bit("Odd")};
    ui->CmbParity->addItems(checklist);
    ui->CmbParity->setCurrentIndex(0);

    QStringList stoplist = {QString::fromLocal8Bit("1"),
                            QString::fromLocal8Bit("1.5"),
                            QString::fromLocal8Bit("2")};
    ui->CmbStopBits->addItems(stoplist);
    ui->CmbStopBits->setCurrentIndex(0);

    on_ptn_refresh_clicked();

    myPort = NULL;
    myPort = new QSerialPort(this);

    connect(myPort, &QSerialPort::readyRead, this, &PortWidgets::receive_data);
}

PortWidgets::~PortWidgets()
{
    delete ui;
}

void PortWidgets::on_ptn_refresh_clicked()
{
    QStringList portNameList;
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    ui->CmPortlist->clear();
    // 遍历所有可用的串行端口信息
    for (const QSerialPortInfo &info : serialPortInfos) {
        portNameList.append(info.portName()); // 添加端口名到列表中
    }
    // 将端口名列表添加到下拉框中
    ui->CmPortlist->addItems(portNameList);
    // 默认情况下选择第一个端口
    if (!portNameList.isEmpty()) {
        ui->CmPortlist->setCurrentIndex(0);
    }
}

//串口连接
void PortWidgets::on_ptn_conn_clicked()
{
    myPort->setPortName(ui->CmPortlist->currentText());

    int baudindex = ui->CmbBaud->currentIndex();
    myPort->setBaudRate(baudRateMap[baudindex]);

    int dataindex = ui->CmbDataBits->currentIndex();
    myPort->setDataBits(dataMap[dataindex]);

    int stopindex = ui->CmbStopBits->currentIndex();
    myPort->setStopBits(stopMap[stopindex]);

    int checkindex = ui->CmbParity->currentIndex();
    myPort->setParity(parityMap[checkindex]);

    myPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!myPort->isOpen())
    {
        myPort->open(QIODevice::ReadWrite);
        if (myPort->isOpen())
            qDebug() << "Connection success!";
        else
            qDebug() << "Connection failed!";
    }
}

//接收来自串口的数据，发送到主串口
void PortWidgets::receive_data()
{
    QByteArray data = myPort->readAll();
    emit receivedata(data);
}

//下发命令
void PortWidgets::write_data(QByteArray writedata)
{
    myPort->write(writedata);
}

