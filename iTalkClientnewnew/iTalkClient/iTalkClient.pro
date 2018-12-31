#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T19:08:39
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iTalkClient
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    form.cpp \
    talk.cpp \
    one.cpp \
    two.cpp \
    mylistwidget.cpp \
    drawer.cpp \
    chat.cpp \
    mychatwidget.cpp \
    wechat.cpp \
    search.cpp \
    emotion.cpp \
    mymessagebox.cpp \
    sunshine.cpp \
    mythread.cpp \
    friendmsg.cpp \
    register.cpp \
    information.cpp \
    register2.cpp \
    register3.cpp \
    groupmsg.cpp \
    qtcpsockethelper.cpp \
    creategroup.cpp \
    fileinput.cpp \
    memberinfo.cpp \
    filenotify.cpp

HEADERS  += widget.h \
    form.h \
    talk.h \
    one.h \
    two.h \
    mylistwidget.h \
    drawer.h \
    chat.h \
    mychatwidget.h \
    wechat.h \
    search.h \
    emotion.h \
    mymessagebox.h \
    sunshine.h \
    mythread.h \
    friendmsg.h \
    register.h \
    information.h \
    register2.h \
    register3.h \
    groupmsg.h \
    qtcpsockethelper.h \
    creategroup.h \
    fileinput.h \
    memberinfo.h \
    filenotify.h

FORMS    += widget.ui \
    form.ui \
    talk.ui \
    one.ui \
    two.ui \
    mylistwidget.ui \
    chat.ui \
    mychatwidget.ui \
    wechat.ui \
    search.ui \
    emotion.ui \
    mymessagebox.ui \
    sunshine.ui \
    friendmsg.ui \
    register.ui \
    register2.ui \
    register3.ui \
    groupmsg.ui \
    qtcpsockethelper.ui \
    creategroup.ui \
    fileinput.ui \
    memberinfo.ui \
    filenotify.ui

RESOURCES += \
    resource.qrc \
    imagesrc.qrc
RC_FILE = myapp.rc
