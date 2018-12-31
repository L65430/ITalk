#include "register3.h"
#include "ui_register3.h"
#include<form.h>
#include<information.h>

Register3::Register3(QString name,QString password,QString sign,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),name(name),password(password),sign(sign),
    ui(new Ui::Register3)
{
    ui->setupUi(this);
    i=0;
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    QMovie *movie = new QMovie(":/loginlogo.gif");
    ui->label->setMovie(movie);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    movie->start();
    QString qss;
      QFile qssFile(":/myqss/login.qss");
      qssFile.open(QFile::ReadOnly);
      //生成随机数
      qDebug()<<"sdfasd";
      if(qssFile.isOpen())
      {

        qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);
        qssFile.close();
        qDebug()<<"成功引入qss";
      }

      QTime time;
      time=QTime::currentTime();
      qsrand(time.msec()+time.second()*1000);
      int result=qrand()%9999;
      resultfinal=QString::number(result,10);
      ui->label_4->setText("您已成功注册，id为:"+resultfinal);
}

Register3::~Register3()
{
    delete ui;
}

bool Register3::eventFilter(QObject* object,QEvent* e){
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_3){
        //关掉当前页面
        close();
    }
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2){
        showMinimized();
        qDebug()<<"最小化";
    }
    return false;
}


void Register3::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
  //  qDebug()<<"mouse_press="<<event->globalPos();
        if(mouse_press)
        {
            i++;
            //鼠标相对于屏幕的位置
            QPoint move_pos = event->globalPos();
             qDebug()<<"移动"<<i;
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}

void Register3::mousePressEvent(QMouseEvent *event)
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


void Register3::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Register3::showEvent(QShowEvent *event){
    mouse_press=false;
}

void Register3::on_pushButton_clicked(){

    QJsonObject json;
    json.insert("name",name);
    json.insert("password",password);
    json.insert("sign",sign);
    json.insert("userid",resultfinal);
    json.insert("headimage","");//头像
    //发送
    QString extra="";
    QJsonDocument document;
    QByteArray messageSent;
    QByteArray block;
    document.setObject(json);
    messageSent = document.toJson(QJsonDocument::Compact);
    //文件的大小
    qint64 totalbytes=messageSent.size();
    QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out<<qint64 (0);
    out<<qint64 (0);
    out<<Information::CLIENT_REGISTER;
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
    tcpSocket->disconnectFromHost();//表示断掉当前的连接
    Form* form=new Form();
    setVisible(false);
    form->show();


//    QJsonDocument document;
//    QByteArray messageSent;
//    QByteArray block;
//    document.setObject(json);
//    messageSent = document.toJson(QJsonDocument::Compact);

//    QDataStream out(&block,QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//    out<<(quint16) 0;
//    out<<Information::CLIENT_REGISTER;
//    out<<messageSent;
//    out.device()->seek(0);
//    out<<(quint16) (block.size() - sizeof(quint16));
//    tcpSocket->write(block);
//    tcpSocket->disconnectFromHost();//表示断掉当前的连接
//    Form* form=new Form();
//    setVisible(false);
//    form->show();
}



