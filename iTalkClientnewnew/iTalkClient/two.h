#ifndef TWO_H
#define TWO_H

#include <QWidget>
#include<QListWidgetItem>
#include<QTcpSocket>
#include"mychatwidget.h"
namespace Ui {
class Two;
}

class Two : public QWidget
{
    Q_OBJECT

public:
    explicit Two(QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Two();
    void additem(QString name,QString sign,QString id);//传入additem的信息
private:
    Ui::Two *ui;
    bool flag=false;
    QTcpSocket* tcpSocket;
    QVector<Mychatwidget*> drawlist;
    QListWidget* listWidget;

public slots:
    void changeheight(QListWidgetItem* item);
    void showitemclick(QListWidgetItem *item);

private slots:
    void on_pushButton_clicked();
};

#endif // TWO_H
