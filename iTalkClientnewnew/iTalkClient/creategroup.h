#ifndef CREATEGROUP_H
#define CREATEGROUP_H

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
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>


namespace Ui {
class CreateGroup;
}

class CreateGroup : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroup(QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~CreateGroup();
    QPoint move_point; //移动的距离

        bool mouse_press; //鼠标按下
        //鼠标按下事件
        void mousePressEvent(QMouseEvent *event);
       //鼠标释放事件
        void mouseReleaseEvent(QMouseEvent *event);
       //鼠标移动事件
        void mouseMoveEvent(QMouseEvent *event);
        //最小化及关闭
     bool eventFilter(QObject *object, QEvent *e);


private:
    Ui::CreateGroup *ui;
    QTcpSocket* tcpSocket;
    int i;

protected:
     void showEvent(QShowEvent *event);

private slots:
    void on_pushButton_clicked();
};

#endif // CREATEGROUP_H
