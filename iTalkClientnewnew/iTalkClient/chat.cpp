#include "chat.h"
#include "ui_chat.h"
#include<QDesktopWidget>
#include"information.h"
#include<QTimer>
Chat::Chat(QString name,QString id,QString sign,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),name(name),id(id),sign(sign),tcpSocket(tcpSocket),
    ui(new Ui::Chat)
{

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->label_2->setText(name);
    ui->label_9->setText("ID:"+id);
    ui->label_3->setText(sign);
    ui->label_4->installEventFilter(this);
    ui->label_5->installEventFilter(this);
    QFile file(":/myqss/scrollbar.qss");
    file.open(QFile::ReadOnly);
    ui->textBrowser->verticalScrollBar()->setStyleSheet(file.readAll());
    ui->label_8->installEventFilter(this);
    ui->label_7->installEventFilter(this);
    move((QApplication::desktop()->width() - width())/2,(QApplication::desktop()->height() - height()-20)/2);
//    emotion=new Emotion();
//    connect(emotion,SIGNAL(imgurl(QString)),this,SLOT(sendemotion(QString)));

    //按时检查有没有人发来回复
    QTimer* timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(checkresponde()));
    timer->start(1000);

}

Chat::~Chat()
{
    delete ui;
}
void Chat::mousePressEvent(QMouseEvent *event)
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

void Chat::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Chat::mouseMoveEvent(QMouseEvent *event)
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

bool Chat::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_5)
    {
        //从正在聊天列表中删除
        for(int i=0;i<Information::getInstance()->talklist.size();i++){
            if(Information::getInstance()->talklist.at(i)==id){//把id这个删掉
               Information::getInstance()->talklist.remove(i);//删除这个一个
            }
        }
        hide();


    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_4){
        showMinimized();
        qDebug()<<"最小化";
        qDebug()<<mouse_press;

    }
    //音频发送
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_8){

//        qDebug()<<ui->label_8->x();
//        qDebug()<<x();
//        emotion->move(x()-10,ui->label_8->y()-100);
//        emotion->show();
//        //显示emotion
        //发送音频

    }
    //文件发送
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_7){
        //新建一个界面用来显示文件，发送文件，然后被服务器接受，要在上面显示出来
        //同时通知对方有一个文件消息进来了，要显示一下
        //选择文件的过程
        Chat::sendtext("[系统消息]发送了一个文件");

    }
    return false;
}
void Chat::showEvent(QShowEvent *event)
{
     qDebug()<<"出现了";
     mouse_press=false;
     qDebug()<<mouse_press;
}

void Chat::on_pushButton_2_clicked()
{
   QString content=ui->textEdit->toPlainText();
   sendtext(content);//就是显示结果在页面上
   //传给服务器
   QJsonObject json;
   json.insert("userid",Information::getInstance()->userid);
   json.insert("friendid",id);
   json.insert("username",Information::getInstance()->name);
   json.insert("bodyvalue",content);
   json.insert("bodytype","text");
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
   out<<Information::CLIENT_CHAT;
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


   //发送个人信息过去
}
//void Chat::sendemotion(QString s)
//{
//    qDebug()<<"选择了发送表情";
//    QString s1="<img width='30' height='30' src='qrc:/10.jpeg' style='float:right;margin-left:15px;margin-right:15px;'><div style='background:rgb(205,215,226);padding-top:8px;float:right;margin-top:10px;margin-right:45px;padding:10px;margin-left:387px;'><img src='"+s+"'></div><br><br><br>";
//    ui->textBrowser->insertHtml(s1);
//}

void Chat::on_pushButton_clicked(){
    for(int i=0;i<Information::getInstance()->talklist.size();i++){
        if(Information::getInstance()->talklist.at(i)==id){//把id这个删掉
           Information::getInstance()->talklist.remove(i);//删除这个一个
        }
    }
    hide();
}

void Chat::sendtext(QString content){
    qDebug()<<content.length();
    if(content.length()>=23){
        qDebug()<<"进入了这个";
      QString s1="<img width='30' height='30' src='qrc:/10.jpeg' style='float:right;margin-left:15px;margin-right:15px;'><div style='background:rgb(205,215,226);padding-top:8px;float:right;margin-top:10px;margin-right:45px;padding:10px;margin-left:60px;'><span style='font-size:15px;font-family:'Microsoft YaHei';margin-top:2px;'>"+content+"</span></div><br><br><br>";
     ui->textBrowser->insertHtml(s1);
    }
    else{
        int flag=60+(24-content.length())*15;
        QString myflag=QString::number(flag, 10);;
        qDebug()<<myflag;
        QString s1="<img width='30' height='30' src='qrc:/10.jpeg' style='float:right;margin-left:15px;margin-right:15px;'><div style='background:rgb(205,215,226);padding-top:8px;float:right;margin-top:10px;margin-right:45px;padding:10px;margin-left:"+myflag+"px;'>";
        s1+="<span style='font-size:15px;font-family:'Microsoft YaHei';margin-top:2px;'>"+content+"</span></div><br><br><br>";
        qDebug()<<s1;
       ui->textBrowser->insertHtml(s1);
    }
    ui->textEdit->clear();
}


void Chat::gettext(QString content){
    qDebug()<<content.length();
    if(content.length()>=23){
        qDebug()<<"进入了这个";
      QString s1="<img width='30' height='30' src='qrc:/10.jpeg' style='float:left;margin-left:15px;margin-right:15px;'><div style='background:rgb(205,215,226);padding-top:8px;float:left;margin-top:10px;margin-right:60px;padding:10px;margin-left:45px;'><span style='font-size:15px;font-family:'Microsoft YaHei';margin-top:2px;'>"+content+"</span></div><br><br><br>";
     ui->textBrowser->insertHtml(s1);
    }
    else{
        int flag=60+(24-content.length())*15;
        QString myflag=QString::number(flag, 10);;
        qDebug()<<myflag;
        QString s1="<img width='30' height='30' src='qrc:/10.jpeg' style='float:left;margin-left:15px;margin-right:15px;'><div style='background:rgb(205,215,226);padding-top:8px;float:left;margin-top:10px;margin-left:45px;padding:10px;margin-right:"+myflag+"px;'>";
        s1+="<span style='font-size:15px;font-family:'Microsoft YaHei';margin-top:2px;'>"+content+"</span></div><br><br><br>";
        qDebug()<<s1;
       ui->textBrowser->insertHtml(s1);
    }
}

//在这里需要一个轮询，不断查找有没有发给自己的信息
void Chat::checkresponde(){
   if(Information::getInstance()->talkmap.contains(id)){
       gettext(Information::getInstance()->talkmap.take(id));
       Information::getInstance()->talkmap.remove(id);
   }
}
