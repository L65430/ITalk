#include "wechat.h"
#include "ui_wechat.h"
#include"information.h"
#include"memberinfo.h"

Wechat::Wechat(QString id,QString name,QString sign,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),id(id),name(name),sign(sign),tcpSocket(tcpSocket),
    ui(new Ui::Wechat)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_6->setText(name);
    ui->label_7->setText(sign);
    ui->label_10->setText("ID:"+id);
    QFile file(":/myqss/scrollbar.qss");
    file.open(QFile::ReadOnly);
    ui->textBrowser->verticalScrollBar()->setStyleSheet(file.readAll());
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_8->installEventFilter(this);
    ui->label_9->installEventFilter(this);

    QTimer* timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(checkresponde()));
    timer->start(1000);

    //在listwidget里面要插入信息
    //每次进来都要更新一下
    //把信息插进去
//    ui->listWidget_2->setStyleSheet("QListWidget::item:hover {background-color:#FCF0C1;}");
    //显示在界面上

    //获取了memberid和state的关系
    //进来就additem
    QMap<QString,QString> memberid_memberstate=Information::getInstance()->groupid_memberid_mermberstate.value(id);
    QMap<QString,QString> memberid_membername=Information::getInstance()->groupid_memberid_membername.value(id);
    QMap<QString,QString>::iterator it;
    for(it=memberid_memberstate.begin();it!=memberid_memberstate.end();++it){
        QString resultid=it.key();
        QString resultstate=memberid_memberstate.value(resultid);
        qDebug()<<"resultstate:"<<resultstate;
        if(memberid_membername.contains(resultid)){
           QString resultname=memberid_membername.value(resultid);
           //把结果输入其中
           additem(resultid,resultname,resultstate);
        }
    }



}

Wechat::~Wechat()
{
    delete ui;
}

void Wechat::mousePressEvent(QMouseEvent *event)
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

void Wechat::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Wechat::mouseMoveEvent(QMouseEvent *event)
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

bool Wechat::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3)
    {
        //关闭然后从里面消失
        //消除在当前界面这个情况
        for(int i=0;i<Information::getInstance()->grouplist.size();i++){
            if(Information::getInstance()->grouplist.at(i)==id){//把id这个删掉
               Information::getInstance()->grouplist.remove(i);//删除这个一个
            }
        }
        hide();
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2){
        showMinimized();
        qDebug()<<"最小化";
        qDebug()<<mouse_press;

    }
    //上传文件
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_8){


    }

    //音频发送
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_9){

    }


    return false;
}
void Wechat::showEvent(QShowEvent *event)
{
     qDebug()<<"出现了";
     mouse_press=false;
     qDebug()<<mouse_press;
}

void  Wechat::on_pushButton_clicked(){
    for(int i=0;i<Information::getInstance()->grouplist.size();i++){
        if(Information::getInstance()->grouplist.at(i)==id){//把id这个删掉
           Information::getInstance()->grouplist.remove(i);//删除这个一个
        }
    }
    hide();
}

void Wechat::sendtext(QString content){
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


void Wechat::gettext(QString content){
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

//此处需要一个轮询，来获取群组消息，显示出来
void Wechat::checkresponde(){
   if(Information::getInstance()->groupmap.contains(id)){
       gettext(Information::getInstance()->groupmap.take(id));
       Information::getInstance()->groupmap.remove(id);
   }
}

//点击之后发送消息
void Wechat::on_pushButton_2_clicked(){
    QString content=ui->textEdit->toPlainText();
    sendtext(content);
    QJsonObject json;
    json.insert("userid",Information::getInstance()->userid);
    json.insert("groupid",id);
    json.insert("groupname",name);
    json.insert("bodyvalue",content);
    json.insert("bodytype","text");
    json.insert("username",Information::getInstance()->name);
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
    out<<Information::CLIENT_GROUP_CHAT_ACTIVE;
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

void Wechat::additem(QString id,QString name,QString state){
    QListWidgetItem* item2=new QListWidgetItem();
    item2->setSizeHint(QSize(0,70));
    Memberinfo* m1=new Memberinfo();
    m1->init(id,name,state);
    ui->listWidget_2->addItem(item2);
    ui->listWidget_2->setItemWidget(item2,m1);
    //加入其中
}


