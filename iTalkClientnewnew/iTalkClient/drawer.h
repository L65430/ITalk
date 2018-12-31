#ifndef DRAWER_H
#define DRAWER_H

#include <QObject>
#include <QToolBox>
#include <QToolButton>
#include<QListWidgetItem>
#include <QTcpSocket>
#include<vector>
#include<QList>
#include<QVector>
#include"mylistwidget.h"
class Drawer : public QToolBox
{
    Q_OBJECT
public:
    Drawer(QWidget *parent=0,Qt::WindowFlags f=0);
    void addfriend(QString name,QString sign,QString id);
    void handletcpsocket(QTcpSocket* tcpSocket1);
private:

    QListWidget* listWidget;
    QListWidget* listWidget2;
    QTcpSocket* tcpSocket;
    QVector<Mylistwidget*> drawlist;
public slots:
    void showitemclick(QListWidgetItem *item);

};

#endif // DRAWER_H
