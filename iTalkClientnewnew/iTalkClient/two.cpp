#include "two.h"
#include "ui_two.h"
#include"mylistwidget.h"
#include "QListWidgetItem"
#include<QListWidget>
#include<QVBoxLayout>
#include<mychatwidget.h>
#include<QDebug>
#include<QTimer>
#include "creategroup.h"
#include "wechat.h"
Two::Two(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),
    ui(new Ui::Two)
{
    ui->setupUi(this);
    QVBoxLayout* layout=new QVBoxLayout(ui->listWidget);


    ui->listWidget->setStyleSheet("QListWidget::item:hover {background-color:#FCF0C1;}");
//    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),m1,SLOT(changehight()));
//    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),m2,SLOT(changehight()));
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(changeheight(QListWidgetItem*)));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(showitemclick(QListWidgetItem*)));
//    QTimer* timer=new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,SLOT(check()));
//    timer->start(1000);
    this->setStyleSheet("QToolBox::tab {background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);border-radius: 2px;color: black;}QToolBox::tab:selected { /* italicize selected tabs */font: italic;color: white; }");
}

Two::~Two()
{
    delete ui;
}
void Two::changeheight(QListWidgetItem* item)
{
    qDebug()<<"进入了";
    item->setSizeHint(QSize(0,60));
//    flag=true;
}

//void Two::check(){
//    qDebug()<<"flag:"<<flag;
//    if(flag){
//       for(int i=0;i<ui->listWidget->count();i++){
//           ui->listWidget->takeItem(i)->sett
//       }
//    }
//}

void Two::on_pushButton_clicked(){
     CreateGroup* creategroup=new CreateGroup(tcpSocket);
     creategroup->show();
}

void Two::additem(QString name,QString sign,QString id){
    QListWidgetItem* item2=new QListWidgetItem();
    item2->setSizeHint(QSize(0,35));
     Mychatwidget* m1=new Mychatwidget();
     m1->init(name,sign,id);
     ui->listWidget->addItem(item2);
     ui->listWidget->setItemWidget(item2,m1);
     drawlist.push_back(m1);
}

void Two::showitemclick(QListWidgetItem *item){
     qDebug()<<ui->listWidget->row(item);
     QString resultid=drawlist.at(ui->listWidget->row(item))->getid();
     QString resultname=drawlist.at(ui->listWidget->row(item))->getname();
     QString resultsign=drawlist.at(ui->listWidget->row(item))->getsign();

     //在这里添加到聊天列表中去
     Information::getInstance()->grouplist.push_back(resultid);
     //传到群组聊天界面里面去
     Wechat* ourchat=new Wechat(resultid,resultname,resultsign,tcpSocket);
     ourchat->show();
}


