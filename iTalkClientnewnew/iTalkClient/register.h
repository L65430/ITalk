#ifndef REGISTER_H
#define REGISTER_H

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
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Register();
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
//  void on_lineEdit_2_returnPressed();
//  void on_lineEdit_textEdited(const QString &arg1);

private:
    int i;
    Ui::Register *ui;
    void openfile();//打开文件
    QString fileName;//保存文件路径
    QTcpSocket* tcpSocket;

protected:
     void showEvent(QShowEvent *event);
private:
};


#endif // REGISTER_H
