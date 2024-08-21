#include "PortWidgets.h"
#include "qserialportinfo.h"
#include "ui_PortWidgets.h"
#include <QString>
#include <QStringList>
#include <qDebug>
#include <QDebug>  // 使用正确的大小写

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif
PortWidgets* PortWidgets::instance = nullptr;
PortWidgets::PortWidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PortWidgets)
{
    // 在构造函数中初始化静态实例指针
    instance = this;
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
    // 在析构函数中清除静态实例指针
    instance = nullptr;
    delete ui;
}

// 生成的 JNI 方法名规则：Java_<包名>_<类名>_<方法名>
// 包名中的 '.' 替换为 '_', 类名与方法名直接用下划线连接

extern "C" JNIEXPORT void JNICALL
Java_org_qtproject_example_UsbController_onDataReceivedFromJava(JNIEnv *env, jobject, jbyteArray data) {

    // 将 jbyteArray 转换为 QByteArray
    jsize length = env->GetArrayLength(data);
    jbyte* byteArray = env->GetByteArrayElements(data, nullptr);
    QByteArray receivedData((const char*)byteArray, length);

    // 使用静态实例指针访问 PortWidgets 实例
    if (PortWidgets::instance) {
        PortWidgets::instance->onDataReceivedFromJava(receivedData);
    }
    qDebug() << "Received data from Java:" << receivedData;
    env->ReleaseByteArrayElements(data, byteArray, 0);
}

void PortWidgets::onDataReceivedFromJava(const QByteArray &data)
{
    // 在这里处理从 Java 传回的串口数据
    qDebug() << "Received data from Java:" << data;
    emit receivedata(data);
}

void PortWidgets::on_ptn_refresh_clicked() {
    QAndroidJniObject javaUsbController = QAndroidJniObject("org/qtproject/example/UsbController",
                                                            "(Landroid/content/Context;)V",
                                                            QtAndroid::androidContext().object());

    QAndroidJniObject result = javaUsbController.callObjectMethod("getAllSerialPort", "()Ljava/util/List;");
    QAndroidJniEnvironment env;
    jobject listObject = result.object<jobject>();

    jclass listClass = env->FindClass("java/util/List");
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

    int size = env->CallIntMethod(listObject, sizeMethod);
    QStringList deviceList;
    for (int i = 0; i < size; i++) {
        jobject serialPortObject = env->CallObjectMethod(listObject, getMethod, i);
        QAndroidJniObject usbDevice = QAndroidJniObject::fromLocalRef(serialPortObject).callObjectMethod("getDevice", "()Landroid/hardware/usb/UsbDevice;");
        QString deviceName = usbDevice.callObjectMethod("getDeviceName", "()Ljava/lang/String;").toString();
        deviceList << deviceName;
        qDebug() << "Serial Port found: " << deviceName;
    }

    ui->CmPortlist->clear();  // 清除旧的列表项
    ui->CmPortlist->addItems(deviceList);  // 添加新的端口列表
    if (!deviceList.isEmpty()) {
        ui->CmPortlist->setCurrentIndex(0);  // 默认选择第一个端口
    }
}



//串口连接
void PortWidgets::on_ptn_conn_clicked()
{
    int portIndex = 0;
    int baudRate = 9600; // 示例波特率
    int dataBits = 8; // 示例数据位
    int stopBits = 1; // 示例停止位
    int parity = 0; // 示例校验位，无校验

    // 创建 Java 的 UsbController 对象
    QAndroidJniObject javaUsbController = QAndroidJniObject("org/qtproject/example/UsbController",
                                                            "(Landroid/content/Context;)V",
                                                            QtAndroid::androidContext().object());

    // 获取所有串行端口的列表
    QAndroidJniObject serialPorts = javaUsbController.callObjectMethod("getAllSerialPort", "()Ljava/util/List;");
    QAndroidJniEnvironment env;
    jobject listObject = serialPorts.object<jobject>();

    // 获取特定索引的串行端口对象
    jobject serialPortObject = env->CallObjectMethod(listObject, env->GetMethodID(env->GetObjectClass(listObject), "get", "(I)Ljava/lang/Object;"), portIndex);

    // 调用 Java 中的 requestPermissionAndOpenPort 方法
    javaUsbController.callMethod<void>("requestPermissionAndOpenPort",
                                       "(Lcom/hoho/android/usbserial/driver/UsbSerialPort;IIII)V",
                                       serialPortObject,
                                       baudRate,
                                       dataBits,
                                       stopBits,
                                       parity);
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


void PortWidgets::on_pushButton_clicked()
{
    // 关闭窗口
    this->close();
}

