QT -= gui
QT += network
QT += sql

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    database.cpp \
    method1.cpp

HEADERS += \
    mytcpserver.h \
    database.h \
    method.h \
    method1.h \
    method_stub.h \
    method_factory.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target