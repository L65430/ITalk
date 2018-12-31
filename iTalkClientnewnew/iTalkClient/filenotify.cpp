#include "filenotify.h"
#include "ui_filenotify.h"
#include"information.h"
#include<QNetworkRequest>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkReply>
#include<QJsonArray>

Filenotify::Filenotify(QString filename,QString username,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),filename(filename),username(username),tcpSocket(tcpSocket),
    ui(new Ui::Filenotify)
{
    ui->setupUi(this);
    ui->label_3->setText(username);
    ui->label_4->setText(filename+" 待接收");
}

Filenotify::~Filenotify()
{
    delete ui;
}


//接受了请求
void Filenotify::on_pushButton_clicked(){
    QJsonObject json;
    json.insert("userid",Information::getInstance()->userid);
    json.insert("filename",filename);


    QJsonDocument document;
    QByteArray messageSent;
    QByteArray block;
    document.setObject(json);
    messageSent = document.toJson(QJsonDocument::Compact);
    //文件的大小
    QString extra="";
    qint64 totalbytes=messageSent.size();
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out<<qint64 (0);
    out<<qint64 (0);
    out<<Information::CLIENT_CHAT_RECEIVE_FILE;
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
}

void Filenotify::on_pushButton_2_clicked(){
    setVisible(false);
}
