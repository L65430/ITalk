#-------------------------------------------------
#
# Project created by QtCreator 2017-07-20T14:06:40
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iTalkServer
TEMPLATE = app


SOURCES += main.cpp\
        server.cpp \
    serverconnectdatabase.cpp \
    databaseoperation.cpp

HEADERS  += server.h \
    serverconnectdatabase.h \
    databaseoperation.h

FORMS    += server.ui
