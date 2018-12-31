#ifndef WECHAT_H
#define WECHAT_H

#include <QWidget>
#include<QMouseEvent>
#include<QtGui>
#include<QDebug>
#include<QScrollBar>
#include<QFile>
#include<QTcpSocket>
namespace Ui {
class Wechat;
}

class Wechat : public QWidget
{
    Q_OBJECT

public:
    explicit Wechat(QString id,QString name,QString sign,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Wechat();
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
     void additem(QString id,QString name,QString state);
protected:
     void showEvent(QShowEvent *event);

private:
    Ui::Wechat *ui;
    QString id;
    QString name;
    QString sign;
    QTcpSocket* tcpSocket;

private slots:
    void on_pushButton_2_clicked();
//     void sendemotion(QString s);
    void on_pushButton_clicked();
    void sendtext(QString content);
    void checkresponde();
    void gettext(QString content);

};

#endif // WECHAT_H
