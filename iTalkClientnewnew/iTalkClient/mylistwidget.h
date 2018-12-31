#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QWidget>
#include <QTcpSocket>
namespace Ui {
class Mylistwidget;
}

class Mylistwidget : public QWidget
{
    Q_OBJECT

public:
    explicit Mylistwidget(QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Mylistwidget();
    void init(QString name,QString sign,QString id,int isperson);
    QString getid();
    QString getname();
    QString getsign();
public:
    Ui::Mylistwidget *ui;
private:
    int isgroup;//为1的时候是点对点，为0的时候是群组
    QString username;
    QString userid;
    QString usersign;
    QTcpSocket* tcpSocket;
private slots:
    void on_pushButton_clicked();
};

#endif // MYLISTWIDGET_H
