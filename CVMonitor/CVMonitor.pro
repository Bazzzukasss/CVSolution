#-------------------------------------------------
#
# Project created by QtCreator 2019-07-24T18:21:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CVMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += ../http-server
INCLUDEPATH += /usr/include

LIBS += -L"../lib"
LIBS += -lqhttpserver

SOURCES += \
    app_configurator.cpp \
    app_settings.cpp \
    coco_item.cpp \
    custom_data.cpp \
    custom_setting.cpp \
    custom_widgets.cpp \
    cv_label.cpp \
    cv_monitor.cpp \
    main.cpp \
    main_window.cpp

HEADERS += \
    app_configurator.h \
    app_constants.h \
    app_settings.h \
    coco_item.h \
    custom_data.h \
    custom_setting.h \
    custom_widgets.h \
    cv_label.h \
    cv_monitor.h \
    main_window.h

FORMS += \
    cv_monitor.ui \
    main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
