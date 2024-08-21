#include "PortWidgets.h"
#include "qserialportinfo.h"
#include "ui_PortWidgets.h"
#include <QString>
#include <QStringList>
#include <qDebug>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

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
    QAndroidJniObject javaUsbController = QAndroidJniObject("org/qtproject/example/UsbController",
                                                            "(Landroid/content/Context;)V",
                                                            QtAndroid::androidContext().object());

    QAndroidJniObject result = javaUsbController.callObjectMethod("getDeviceList", "()[Ljava/lang/String;");
    QAndroidJniEnvironment env;
    jobjectArray arr = result.object<jobjectArray>();
    int count = env->GetArrayLength(arr);
    QStringList deviceList;
    for (int i = 0; i < count; i++) {
        QAndroidJniObject deviceName = env->GetObjectArrayElement(arr, i);
        deviceList << deviceName.toString();
        qDebug() << "Device found: " << deviceName.toString();
    }
    // 将端口名列表添加到下拉框中
    ui->CmPortlist->addItems(deviceList);
    // 默认情况下选择第一个端口
    if (!deviceList.isEmpty()) {
        ui->CmPortlist->setCurrentIndex(0);
    }
/*    QStringList portNameList;
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
*/
}

//串口连接
void PortWidgets::on_ptn_conn_clicked()
{
/*    // 从 UI 中获取当前选中的端口名并创建一个 Android JNI 字符串对象
    QAndroidJniObject portName = QAndroidJniObject::fromString(ui->CmPortlist->currentText());

    // 静态调用 Java 端的方法。确保类名和方法签名正确
    jboolean isSuccess = QAndroidJniObject::callStaticMethod<jboolean>(
        "usb/USBListActivity",    // Java 类的完全限定名
        "test",                   // 方法名
        "(Ljava/lang/String;)Z",  // 方法签名，接受 String 返回 boolean
        portName.object<jstring>() // 将 QAndroidJniObject 转换为 jstring
        );

    // 根据返回值输出结果
    if (isSuccess) {
        qDebug() << "Test succeeded";
    } else {
        qDebug() << "Test failed";
    }
*/
//    myPort->setPortName(ui->CmPortlist->currentText());

//    int baudindex = ui->CmbBaud->currentIndex();
//    myPort->setBaudRate(baudRateMap[baudindex]);

//    int dataindex = ui->CmbDataBits->currentIndex();
//    myPort->setDataBits(dataMap[dataindex]);

//    int stopindex = ui->CmbStopBits->currentIndex();
//    myPort->setStopBits(stopMap[stopindex]);

//    int checkindex = ui->CmbParity->currentIndex();
//    myPort->setParity(parityMap[checkindex]);

//    myPort->setFlowControl(QSerialPort::NoFlowControl);

//    if (!myPort->isOpen())
//    {
//        myPort->open(QIODevice::ReadWrite);
//        if (myPort->isOpen())
//            qDebug() << "Connection success!";
//        else
//            qDebug() << "Connection failed!";
//    }

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

