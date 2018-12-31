#ifndef REGISTER2_H
#define REGISTER2_H

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
#include "information.h"

namespace Ui {
class Register2;
}

class Register2 : public QWidget
{
    Q_OBJECT

public:
    explicit Register2(QString name,QString password,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Register2();
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
    void on_pushButton_clicked();//点击之后进入下一个结果
private:
    int i=0;
    Ui::Register2 *ui;
    QTcpSocket* tcpSocket;
    QString name;
    QString password;


protected:
    void showEvent(QShowEvent *event);
};

#endif // REGISTER2_H
