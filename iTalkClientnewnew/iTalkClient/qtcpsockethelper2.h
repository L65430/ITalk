#ifndef QTCPSOCKETHELPER2_H
#define QTCPSOCKETHELPER2_H
#include <QWidget>
#include<QLabel>
#include<QMovie>
#include<QMouseEvent>
#include<QDebug>
#include<QPropertyAnimation>
#include<QImage>
#include<QDesktopServices>
#include<QPropertyAnimation>
#include<QNetworkAccessManager>
#include<QCryptographicHash>
#include "form.h"
#include "information.h"
#include "talk.h"

class QTcpSocketHelper2
{
     Q_OBJECT
public:
    QTcpSocketHelper2(QTcpSocket* tcpSocket,QWidget *parent = 0);
private:
    Ui::QTcpSocketHelper *ui;
    QTcpSocket* tcpSocket;
    quint16 blockSize;
    QJsonObject message;
    void login();

public:
     bool login_success=false;
private slots:
    void resolvemessage();
};

#endif // QTCPSOCKETHELPER2_H
