QT += core gui
QT += serialport
QT += androidextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PortWidgets.cpp \
    main.cpp \
    SerialPort.cpp

HEADERS += \
    PortWidgets.h \
    SerialPort.h

FORMS += \
    PortWidgets.ui \
    SerialPort.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/java/UsbController.java \
    android/res/values/libs.xml \
    android/src/com/hoho/android/usbserial/driver/*.java \
    android/src/com/hoho/android/usbserial/util/*.java
