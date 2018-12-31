#ifndef GROUPMSG_H
#define GROUPMSG_H

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
class Groupmsg;
}

class Groupmsg : public QWidget
{
    Q_OBJECT

public:
    explicit Groupmsg(QString name,QString id,QString sign,QString groupid,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Groupmsg();
     bool eventFilter(QObject *object, QEvent *e);
     bool mouse_press;//鼠标
private:
    Ui::Groupmsg *ui;
    QString name;
    QString id;
    QString sign;
    QString groupid;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QJsonObject message;
    Talk* talk;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

protected:
    void showEvent(QShowEvent* event);
};

#endif // GROUPMSG_H
