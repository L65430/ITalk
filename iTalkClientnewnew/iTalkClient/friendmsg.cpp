#include "friendmsg.h"
#include "ui_friendmsg.h"
#include "information.h"
#include<drawer.h>//用来多增加一条

Friendmsg::Friendmsg(QString name,QString id,QString sign,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),name(name),id(id),sign(sign),tcpSocket(tcpSocket),
    ui(new Ui::Friendmsg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    ui->label_6->setText(""+name);
    ui->label_7->setText(""+id);
    ui->label_8->setText(""+sign);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);


}

Friendmsg::~Friendmsg()
{
    delete ui;
}

//同意好友请求
void Friendmsg::on_pushButton_clicked(){
    //发送同意的网络请求，发送userid和friendid，说我同意了
    QJsonObject json;
    json.insert("userid",Information::getInstance()->userid);
    json.insert("friendid",id);
    json.insert("name",Information::getInstance()->name);
    json.insert("sign",Information::getInstance()->sign);
    QJsonDocument document;
    QByteArray messageSent;
    QByteArray block;
    document.setObject(json);
    messageSent = document.toJson(QJsonDocument::Compact);

    qint64 totalbytes=messageSent.size();
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out<<qint64 (0);
    out<<qint64 (0);
    out<<Information::CLIENT_ADD_FRIEND_SUCCESS;
    out<<"";
    totalbytes+=block.size();
    out.device()->seek(0);
    out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
//        out<<totalbytes;
//        out<<messageSent;
    tcpSocket->write(block);
    block.resize(0);//清空缓存区
    block=messageSent;
    tcpSocket->write(block);

    setVisible(false);//显示不可见
    Information::getInstance()->agree_add_friend=true;//同意添加好友
    Information::getInstance()->add_friend_name=name;
    Information::getInstance()->add_friend_sign=sign;
    Information::getInstance()->add_friend_id=id;//获取id
    delete(this);
    //同时更新talk部分
    //显示添加好友
    //talk要轮训查找结果,然后把结果放进去

}

//拒绝好友请求
void Friendmsg::on_pushButton_2_clicked(){
    //拒绝，告诉对方已经拒绝了你的好友请求
     QJsonObject json;
     json.insert("userid",Information::getInstance()->userid);
     json.insert("friendid",id);
     json.insert("name",Information::getInstance()->name);
     json.insert("sign",Information::getInstance()->sign);
     QJsonDocument document;
     QByteArray messageSent;
     QByteArray block;
     document.setObject(json);
     messageSent = document.toJson(QJsonDocument::Compact);

     QString extra="";
     qint64 totalbytes=messageSent.size();
     QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
     out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
     out<<qint64 (0);
     out<<qint64 (0);
     out<<Information::CLIENT_ADD_FRIEND_FAIL;
     out<<extra;
     totalbytes+=block.size();
     out.device()->seek(0);
     out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
 //        out<<totalbytes;
 //        out<<messageSent;
     tcpSocket->write(block);
     block.resize(0);//清空缓存区
     block=messageSent;
     tcpSocket->write(block);

     setVisible(false);//显示不可见
     Information::getInstance()->agree_add_group=false;
     delete(this);
}

bool Friendmsg::eventFilter(QObject* object,QEvent* e){
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3){
        //关掉当前页面
        close();
    }
    //这里还要添加一个
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3){
        //关掉当前页面
        close();
    }
}

void Friendmsg::showEvent(QShowEvent *event){
    mouse_press=false;
}

