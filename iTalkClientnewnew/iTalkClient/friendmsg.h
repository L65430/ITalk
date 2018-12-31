#ifndef FRIENDMSG_H
#define FRIENDMSG_H
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
#include<talk.h>

namespace Ui {
class Friendmsg;
}

class Friendmsg : public QWidget
{
    Q_OBJECT

public:
    explicit Friendmsg(QString name,QString id,QString sign,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Friendmsg();
    bool eventFilter(QObject *object, QEvent *e);
    bool mouse_press;//鼠标
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
private:
    Ui::Friendmsg *ui;
    QString name;
    QString id;
    QString sign;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QJsonObject message;
protected:
    void showEvent(QShowEvent* event);
};

#endif // FRIENDMSG_H
