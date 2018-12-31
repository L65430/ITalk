#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include<QMouseEvent>
#include<QtGui>
#include<QDebug>
#include<QFile>
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QJsonObject>
#include<QByteArray>
#include<QNetworkReply>
#include<QTcpSocket>
#include<QTime>

namespace Ui {
class Search;
}
class Search : public QWidget
{
    Q_OBJECT

public:
     int isGroup=0;
     int isPerson=0;
    explicit Search(QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Search();
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
     void doSearch(QString message);
protected:
     void showEvent(QShowEvent *event);
private:
      Ui::Search *ui;
      quint16 blockSize;
      QJsonObject message;
      QTimer* timer;
      QTcpSocket* tcpSocket;
private slots:
     void on_pushButton_2_clicked();
     void on_pushButton_clicked();
//     void parseresult();
     void checkresult();

};

#endif // SEARCH_H
