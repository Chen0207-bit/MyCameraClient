QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CameraInterface/CameraContext.cpp \
    CameraInterface/VirtualCamera.cpp \
    ControlWidget/controlwidget.cpp \
    ParamWidget/paramwidget.cpp \
    ViewWidget/viewwidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    CameraInterface/CMCameraMetaInfo.h \
    CameraInterface/CameraContext.h \
    CameraInterface/CameraError.h \
    CameraInterface/CameraInterface.h \
    CameraInterface/CameraParam.h \
    CameraInterface/VirtualCamera.h \
    ControlWidget/controlwidget.h \
    ParamWidget/paramwidget.h \
    ViewWidget/viewwidget.h \
    mainwindow.h

FORMS += \
    ControlWidget/controlwidget.ui \
    ParamWidget/paramwidget.ui \
    ViewWidget/viewwidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
