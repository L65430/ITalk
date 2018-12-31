#ifndef REGISTER3_H
#define REGISTER3_H
#include <QWidget>
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
#include <QWidget>
#include<QTime>
#include "information.h"

namespace Ui {
class Register3;
}

class Register3 : public QWidget
{
    Q_OBJECT

public:
    explicit Register3(QString name,QString password,QString sign,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Register3();
    QPoint move_point; //移动的距离

    bool mouse_press;//鼠标
    void mousePressEvent(QMouseEvent *event);
   //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
   //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //最小化及关闭
 bool eventFilter(QObject *object, QEvent *e);
private slots:
    void on_pushButton_clicked();

private:
    int i;
    Ui::Register3 *ui;
    QTcpSocket* tcpSocket;
    quint16 blockSize;
    QJsonObject message;
    QString name;
    QString password;
    QString sign;
    QString resultfinal;

protected:
    void showEvent(QShowEvent *event);
};

#endif // REGISTER3_H
