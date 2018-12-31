#include "drawer.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include<QDebug>
#include<mylistwidget.h>
#include<QFile>
#include "two.h"
#include "chat.h"
#include"information.h"
//需要一个tcpsocket传进来，作为结果
Drawer::Drawer(QWidget *parent,Qt::WindowFlags f)
    :QToolBox(parent,0)
{

   //******************添加自定义的列表*****************
       listWidget = new QListWidget;
        listWidget2 = new QListWidget;
        QVBoxLayout *layout4=new QVBoxLayout(listWidget);
        //这里的demo是运用itemclick信号，双击两下之后获取当前item信息，然后
        connect(listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(showitemclick(QListWidgetItem*)));
//         QListWidgetItem *listItem1 = new QListWidgetItem();
//         listWidget->addItem(listItem1);
//         Mylistwidget* mlistwidget=new Mylistwidget();
//         listItem1->setSizeHint(QSize(0, 62));
//         listWidget->setItemWidget(listItem1, mlistwidget);
//         QListWidgetItem *listItem2 = new QListWidgetItem();
//         listWidget->addItem(listItem2);
//         Mylistwidget* mlistwidget2=new Mylistwidget();
//         listItem2->setSizeHint(QSize(0, 62));
//         listWidget->setItemWidget(listItem2, mlistwidget2);



//         list=new list
         Drawer::addfriend("xixix","helloworld","111");
         Drawer::addfriend("haha","helloworld","222");

         listWidget->setStyleSheet("QListWidget::item:hover {background-color:#FCF0C1;}");
         listWidget2->setStyleSheet("QListWidget::item:hover {background-color:#FCF0C1;}");


   //***************添加至主窗口*****************
        this->addItem(listWidget,tr("我的好友"));
          QVBoxLayout *layout1=new QVBoxLayout(listWidget2);
          this->addItem(listWidget2,tr("陌生人"));
        this->setStyleSheet("QToolBox::tab {background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);border-radius: 2px;color: black;}QToolBox::tab:selected { /* italicize selected tabs */font: italic;color: white; }");


}
void Drawer::showitemclick(QListWidgetItem *item)
{
//     qDebug()<<"item信息是:"<<item->text();
//     qDebug()<<listWidget->row(item);
//     //进入结果中去查找
//     //点击之后要及时显示结果出来
    //点击的部分
   qDebug()<<listWidget->row(item);
   QString resultid=drawlist.at(listWidget->row(item))->getid();
   QString resultname=drawlist.at(listWidget->row(item))->getname();
   QString resultsign=drawlist.at(listWidget->row(item))->getsign();
   qDebug()<<resultid;
   //传到聊天里面去
   qDebug()<<resultname;
   //在这里传入一个tcpSocket
   Chat* friendchat=new Chat(resultname,resultid,resultsign,tcpSocket);
   //添加到正在聊天人列表中去
   Information::getInstance()->talklist.push_back(resultid);
   friendchat->show();
}
void Drawer::addfriend(QString name,QString sign,QString id)
{
    QListWidgetItem *listItem = new QListWidgetItem();
    listWidget->addItem(listItem);
    Mylistwidget* mlistwidget=new Mylistwidget(tcpSocket);
    listItem->setSizeHint(QSize(0, 62));
    mlistwidget->init(name,sign,id,2);
    listWidget->setItemWidget(listItem, mlistwidget);
    drawlist.push_back(mlistwidget);
}

void Drawer::handletcpsocket(QTcpSocket* tcpSocket1){
    tcpSocket=tcpSocket1;
}


