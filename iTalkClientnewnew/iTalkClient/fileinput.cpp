#include "fileinput.h"
#include "ui_fileinput.h"
#include<QTextCodec>
#include"information.h"
#include<QFileDialog>
#include<QNetworkRequest>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkReply>
#include<QJsonArray>

Fileinput::Fileinput(QString friendid,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),friendid(friendid),
    ui(new Ui::Fileinput)
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    QMovie *movie = new QMovie(":/loginlogo.gif");
    ui->setupUi(this);
    ui->label->setMovie(movie);
     movie->start();
    //开始的时候隐藏
    ui->label_4->hide();
    ui->pushButton_2->setEnabled(false);//开始的时候不能点击
//     connect(tcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
     connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));

     QString qss;
       QFile qssFile(":/myqss/login.qss");
       qssFile.open(QFile::ReadOnly);
       qDebug()<<"sdfasd";
       if(qssFile.isOpen())
       {

         qss = QLatin1String(qssFile.readAll());
         this->setStyleSheet(qss);
         qssFile.close();
         qDebug()<<"成功引入qss";
       }

       ui->label_2->installEventFilter(this);
       ui->label_3->installEventFilter(this);

}

Fileinput::~Fileinput()
{
    delete ui;
}

//发送信息头文件


//打开文件
void Fileinput::on_pushButton_clicked(){
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        ui->pushButton_2->setEnabled(true);
        ui->label_4->setText(tr("打开文件 %1 成功！").arg(fileName));
    }
}


//发送
void Fileinput::on_pushButton_2_clicked(){
      //直接发送文件


    localFile=new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly)){
        qDebug() << "open file error!";
        return;
    }
    QJsonDocument document;
    QByteArray messageSent;
    QByteArray messageSent2;
    QByteArray block;


    //文件的大小
    QJsonObject json;
    json.insert("filename",fileName.right(fileName.size() - fileName.lastIndexOf('/')-1));
    json.insert("userid",Information::getInstance()->userid);
    json.insert("friendid",friendid);
    json.insert("username",Information::getInstance()->name);
//    QString extra=QString((QJsonDocument(json).toJson()));

    messageSent2=document.toJson(QJsonDocument::Compact);
    QString extra=QString(messageSent2);
    qint64 totalbytes=localFile->size();
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    messageSent = localFile->readAll();
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out<<qint64 (0);
    out<<qint64 (0);
    out<<Information::CLIENT_CHAT_SEND_FILE;
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
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesWritten);
    //更新进度条
    if(bytesWritten == totalBytes) //发送完毕
    {
        ui->label_4->setText(tr("传送文件 %1 成功").arg(fileName));
        localFile->close();
        tcpSocket->close();
    }

}

bool Fileinput::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3)
    {
         close();
         delete(this);
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2){
        hide();
        qDebug()<<"最小化";

    }
    return false;
}






