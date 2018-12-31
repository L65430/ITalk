#include "search.h"
#include "ui_search.h"
#include<information.h>

Search::Search(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),//这里是表示初始化
    ui(new Ui::Search)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_4->installEventFilter(this);
    ui->label_5->installEventFilter(this);

    ui->label_7->setVisible(false);
    ui->label_8->setVisible(false);
    ui->pushButton_2->setVisible(false);

    //在这里传入了一个tcpSocket
    qDebug()<<"开始了调用连接";
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(checkresult()));
    timer->start(1000);

}

Search::~Search()
{
    delete ui;
}

void Search::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
          qDebug()<<"移动1";
          qDebug()<<event->source();
     }

}

void Search::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Search::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
  //  qDebug()<<"mouse_press="<<event->globalPos();
        if(mouse_press)
        {

            //鼠标相对于屏幕的位置
            QPoint move_pos = event->globalPos();
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}

bool Search::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3)
    {
         //只能保证只有一个槽，否则会乱发消息
         this->hide();
         delete(this);
         e->ignore();
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2){
        showMinimized();
        qDebug()<<"最小化";
        qDebug()<<mouse_press;

    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_4){

        ui->label_4->setStyleSheet("color:white;font-size:16px;font-weight:bold;");
        ui->label_5->setStyleSheet("color:white");
        isPerson=1;
        isGroup=0;

    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_5){//点击变换到群组的时候

        ui->label_5->setStyleSheet("color:white;font-size:16px;font-weight:bold;");
        ui->label_4->setStyleSheet("color:white");
        isPerson=0;
        isGroup=1;
    }

    return false;
}

void Search::showEvent(QShowEvent *event)
{
     qDebug()<<"出现了";
     mouse_press=false;
     qDebug()<<mouse_press;
}


//点击这个函数后向服务器发送添加好友请求
void Search::on_pushButton_2_clicked()
{
   QJsonObject json;
   json.insert("userid",Information::getInstance()->userid);
   if(isGroup){
       json.insert("groupid",ui->lineEdit->text());
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
       out<<Information::CLIENT_ADD_GROUP;
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
   if(isPerson){
       json.insert("friendid",ui->lineEdit->text());
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
       out<<Information::CLIENT_ADD_FRIEND;
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
}
void Search::doSearch(QString message)
{
    ui->lineEdit->setText(message);
}

//点击此处发送好友搜索的请求
void Search::on_pushButton_clicked()
{

     QJsonObject json;
     QJsonDocument document;
     QByteArray messagesent;
     QByteArray block;


     if(isGroup){
       //表示搜索群组
         qDebug()<<"input result"<<ui->lineEdit->text();
        json.insert("userid",Information::getInstance()->userid);
        json.insert("groupid",ui->lineEdit->text());
        document.setObject(json);
        messagesent = document.toJson(QJsonDocument::Compact);

        QString extra="";
        qint64 totalbytes=messagesent.size();
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        out<<qint64 (0);
        out<<qint64 (0);
        out<<Information::CLIENT_SEARCH_GROUP;
        out<<extra;
        totalbytes+=block.size();
        out.device()->seek(0);
        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
    //        out<<totalbytes;
    //        out<<messageSent;
        tcpSocket->write(block);
        block.resize(0);//清空缓存区
        block=messagesent;
        tcpSocket->write(block);
   }
     if(isPerson){
         json.insert("userid",Information::getInstance()->userid);
         json.insert("friendid",ui->lineEdit->text());
         document.setObject(json);
         messagesent = document.toJson(QJsonDocument::Compact);

         QString extra="";
         qint64 totalbytes=messagesent.size();
         QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
         out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
         out<<qint64 (0);
         out<<qint64 (0);
         out<<Information::CLIENT_SEARCH_FRIEND;
         out<<extra;
         totalbytes+=block.size();
         out.device()->seek(0);
         out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
     //        out<<totalbytes;
     //        out<<messageSent;
         tcpSocket->write(block);
         block.resize(0);//清空缓存区
         block=messagesent;
         tcpSocket->write(block);
     }
    //分为搜索群组和搜索个人的信息
}


//这里是得到的结果
//void Search::parseresult()
//{
//    blockSize=0;
//    qDebug()<<"进入了search parseresult函数";
//    int DATA_EMENU=0;
//    QByteArray document;
//     QDataStream in(tcpSocket);
//     in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//     //设置数据流版本，这里要和服务器端相同
//     if(blockSize==0) //如果是刚开始接收数据
//     {
//         //判断接收的数据是否有两字节，也就是文件的大小信息
//         //如果有则保存到blockSize变量中，没有则返回，继续接收数据
//         if(tcpSocket->bytesAvailable() < (int)sizeof(quint16))
//             return;
//         in >> blockSize;
//     }
//     if(tcpSocket->bytesAvailable() < blockSize) return;
//     //如果没有得到全部的数据，则返回，继续接收数据
//     in>>DATA_EMENU;
//     qDebug()<<"DATA_EMENU"<<DATA_EMENU;
//     in >>document;
//     QString qba=document.toStdString().c_str();
//     qDebug()<<qba;
//     //将接收到的数据存放到变量中
//     QJsonParseError error;
//     QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
// if(error.error==QJsonParseError::NoError)
// {
//     if(jsonDocument.isObject()){
//       QJsonObject obj=jsonDocument.object();
//       if(DATA_EMENU==Information::getInstance()->SERVER_SEARCH_FRIEND_RESPOND){
//          QJsonValue USERNAME=obj.take("name");
//          ui->label_8->setText(USERNAME.toString());
//          ui->label_7->setVisible(true);
//          ui->label_8->setVisible(true);
//          ui->pushButton_2->setVisible(true);
//       }
//       if(DATA_EMENU==Information::getInstance()->SERVER_SEARCH_GROUP_RESPOND){
//           QJsonValue USERNAME=obj.take("groupname");
//           ui->label_8->setText(USERNAME.toString());
//           ui->label_7->setVisible(true);
//           ui->label_8->setVisible(true);
//           ui->pushButton_2->setVisible(true);
//       }
//     }
//   }
//}

void Search::checkresult(){
    qDebug()<<"此时information中saerch_friend_name是"<<Information::getInstance()->search_friend_name;
    if(Information::getInstance()->search_friend_name!=""){
        ui->label_8->setText(Information::getInstance()->search_friend_name);
        ui->label_7->setVisible(true);
        ui->label_8->setVisible(true);
        ui->pushButton_2->setVisible(true);
        Information::getInstance()->search_friend_name="";
    }
    if(Information::getInstance()->search_group_name!=""){
        ui->label_8->setText(Information::getInstance()->search_group_name);
        ui->label_7->setVisible(true);
        ui->label_8->setVisible(true);
        ui->pushButton_2->setVisible(true);
        Information::getInstance()->search_group_name="";
    }
}
