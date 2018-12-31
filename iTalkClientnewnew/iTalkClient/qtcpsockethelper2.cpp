#include "qtcpsockethelper2.h"

QTcpSocketHelper::QTcpSocketHelper(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),//在这里进行处理
{
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(resolvemessage()));
}
