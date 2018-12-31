#ifndef QTCPSOCKETHELPER_H
#define QTCPSOCKETHELPER_H

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


namespace Ui {
class QTcpSocketHelper;
}

class QTcpSocketHelper : public QWidget
{
    Q_OBJECT

public:
    explicit QTcpSocketHelper(QTcpSocket* tcpSocket,QWidget *parent = 0);//就是传入这个参数
    ~QTcpSocketHelper();

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

#endif // QTCPSOCKETHELPER_H
