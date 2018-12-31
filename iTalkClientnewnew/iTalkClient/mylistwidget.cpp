#include "mylistwidget.h"
#include "ui_mylistwidget.h"
#include"information.h"
#include"chat.h"
#include"wechat.h"

Mylistwidget::Mylistwidget(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),
    ui(new Ui::Mylistwidget)
{
    ui->setupUi(this);
}

Mylistwidget::~Mylistwidget()
{
    delete ui;
}
void Mylistwidget::init(QString name,QString sign,QString id,int group)
{
    ui->label_3->setText(name);
    ui->label_4->setText(sign);
    ui->label_5->setText(id);
    ui->label_5->setVisible(false);
    username=name;
    userid=id;
    usersign=sign;
    isgroup=group;
    if(isgroup==2){
        ui->pushButton->setVisible(false);
    }

}

QString Mylistwidget::getid(){
    return ui->label_5->text();
}

QString Mylistwidget::getsign(){
    return ui->label_4->text();
}

QString Mylistwidget::getname(){
    return ui->label_3->text();
}

//点击之后跳转，就是在里面就有个tcpsocket传进来，然后用那个tcpsocket来写，直接
void Mylistwidget::on_pushButton_clicked(){
    //点击之后发生跳转，如果是群组的话，通知跳转到群组，如果不是的话，通知跳转到结果
    //如何判断这个message是群聊还是什么，在里面加上一部分来表示
    //为1的时候是点对点，为0的时候是群组
    if(isgroup==1){
        //点对点
        Information::getInstance()->talklist.push_back(userid);
        Chat* friendchat=new Chat(username,userid,usersign,tcpSocket);
        friendchat->show();
    }
    if(isgroup==0){
        Information::getInstance()->grouplist.push_back(userid);
        Wechat* ourchat=new Wechat(userid,username,usersign,tcpSocket);
        ourchat->show();
    }

}
