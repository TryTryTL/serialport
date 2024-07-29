#include "SerialPort.h"
#include "ui_SerialPort.h"

#include <QTime>

SerialPort::SerialPort(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SerialPort)
{
    ui->setupUi(this);
    m_portWidget = NULL;
    ui->textEdit_recv->setReadOnly(true);
}

SerialPort::~SerialPort()
{
    delete ui;
}

void SerialPort::on_ptn_conn_clicked()
{
    if (m_portWidget == NULL) {
        m_portWidget = new PortWidgets();
        connect(m_portWidget, &PortWidgets::receivedata, this, &SerialPort::receive_data);//接收来自串口界面的消息
        connect(this, &SerialPort::writedata, m_portWidget, &PortWidgets::write_data);    //发送数据到串口界面
    }
    m_portWidget->showNormal();
}

void SerialPort::on_lineEdit_dataSend_returnPressed()
{
    if (ui->lineEdit_dataSend->text() != NULL)
    {
        int value = ui->lineEdit_dataSend->text().toInt();
        QByteArray data;
        data.resize(1);
        data[0] = value & 0xff;
        emit writedata(data);
    }
}

void SerialPort::on_ptn_clear_clicked()
{
    ui->textEdit_recv->clear();
}

void SerialPort::receive_data(QByteArray data)
{
    // 获取当前的时间戳
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 将 QByteArray 转换为 QString
    QString str = QString::fromLatin1(data);

    // 使用 HTML 设置时间戳的颜色，例如蓝色
    QString coloredTimestamp = QString("<span style=\"color: blue;\">%1</span>").arg(timestamp);

    // 使用 HTML 设置数据的颜色，例如黑色
    QString coloredData = QString("<span style=\"color: black;\">%1</span>").arg(str);

    // 先添加彩色时间戳到文本编辑器
    ui->textEdit_recv->append(coloredTimestamp);

    // 然后添加彩色数据到文本编辑器的下一行
    ui->textEdit_recv->append(coloredData);
}
