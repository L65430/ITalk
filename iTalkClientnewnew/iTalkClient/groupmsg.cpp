#include "groupmsg.h"
#include "ui_groupmsg.h"
#include<talk.h>
#include<information.h>

Groupmsg::Groupmsg(QString name,QString id,QString sign,QString groupid,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),name(name),id(id),sign(sign),tcpSocket(tcpSocket),groupid(groupid),
    ui(new Ui::Groupmsg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    ui->label_6->setText("姓名："+name);
    ui->label_7->setText("id:"+id);
    ui->label_8->setText("签名："+sign);
    ui->label_9->setText("加入群"+groupid);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);

}

Groupmsg::~Groupmsg()
{
    delete ui;
}

//同意加入群组请求
void Groupmsg::on_pushButton_clicked(){
    QJsonObject json;
    json.insert("groupid",groupid);
    json.insert("friendid",id);
    json.insert("userid",Information::getInstance()->userid);
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
    out<<Information::CLIENT_ADD_GROUP_SUCCESS;
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
    //要添加到群组中去，群组不需要改变，但是在进入群组界面的时候需要改变
}

//点击了拒绝
void Groupmsg::on_pushButton_2_clicked(){
    QJsonObject json;
    json.insert("groupid",groupid);
    json.insert("friendid",id);
    json.insert("userid",Information::getInstance()->userid);
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
    out<<Information::CLIENT_ADD_GROUP_FAIL;
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
}


bool Groupmsg::eventFilter(QObject* object,QEvent* e){
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3){
        //关掉当前页面
        close();
    }
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3){
        //关掉当前页面
        close();
    }
}

void Groupmsg::showEvent(QShowEvent *event){
    mouse_press=false;
}
