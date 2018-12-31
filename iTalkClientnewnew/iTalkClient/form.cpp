#include "form.h"
#include "information.h"
#include "ui_form.h"
#include<QNetworkRequest>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkReply>
#include<QJsonArray>
#include "register.h"
#include "mymessagebox.h"
#include "talk.h"
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    i=0;
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    QMovie *movie = new QMovie(":/loginlogo.gif");
     ui->label->setMovie(movie);
     ui->label_2->installEventFilter(this);
     ui->label_3->installEventFilter(this);
     movie->start();
     //QImage image;
     //image.load(":/10.jpeg");
     //QImage result = image.scaled(85, 85, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
     //ui->label_4->setPixmap(QPixmap::fromImage(result));
     ui->label_4->setStyleSheet("border-radius:15px;background-color:green;border-image:url(':/logo.jpg')");
     ui->label_5->setOpenExternalLinks(true);

     ui->label_5->installEventFilter(this);
     animation=new QPropertyAnimation(ui->label_4, "geometry");
     animation->setDuration(400);
     animation->setStartValue(QRect(30, 170, 85, 85));
     animation->setEndValue(QRect(150, 170, 85, 85));

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
       tcpSocket=new QTcpSocket(this);
       helper=new QTcpSocketHelper(tcpSocket);
       tcpSocket->abort();
       tcpSocket->connectToHost("127.0.0.1",6666);


       timer=new QTimer(this);
       connect(timer,SIGNAL(timeout()),this,SLOT(turntotalk()));
       timer->start(1000);
}

Form::~Form()
{
    delete ui;
}
void Form::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
          qDebug()<<event->source();
     }

}

void Form::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Form::mouseMoveEvent(QMouseEvent *event)
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

bool Form::eventFilter(QObject *object, QEvent *e)
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
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_5){
        Register* r=new Register(tcpSocket);
        setVisible(false);
        r->show();
    }
    return false;
}


void Form::on_pushButton_clicked()
{
    qDebug()<<ui->lineEdit->text();
    if(ui->lineEdit->text()==""||ui->lineEdit_2->text()==""){
        qDebug()<<"内容为空";
        ui->label_6->setText("账号密码不能为空");
    }
    else{
        ui->label_6->hide();
        ui->pushButton->hide();
        ui->lineEdit->hide();
        ui->lineEdit_2->hide();
        ui->label_5->hide();
        animation->start();



        QJsonObject json;
        json.insert("id",ui->lineEdit->text());
        json.insert("password",ui->lineEdit_2->text());


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
        out<<Information::CLIENT_LOGIN;
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



//        Mymessagebox* message=new Mymessagebox();
//        message->show();

    }
//    Talk* t=new Talk();
//    setVisible(false);
//    t->init(byte_array);
//    t->show();

    /*ui->pushButton->hide();
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
    ui->label_5->hide();
    animation->start();*/
}

void Form::on_lineEdit_2_returnPressed()
{
    qDebug()<<"输入结束66";
}

void Form::showEvent(QShowEvent *event)
{
    mouse_press=false;
}

void Form::on_lineEdit_textEdited(const QString &arg1)
{
    qDebug()<<"正在编辑"+arg1;
    if(ui->label_6->text()!=""){
        qDebug()<<"进入了这个函数";
        ui->label_6->setText("");
    }

}

//void Form::readMessage()
//{
//    blockSize=0;
//    qDebug()<<"form readmessage槽触发";
//   // Information*resultinformation=Information::getInstance();
//    int DATA_EMENU=0;
//    QByteArray document;
//    QDataStream in(tcpSocket);
//    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//    //设置数据流版本，这里要和服务器端相同
//    if(blockSize==0) //如果是刚开始接收数据
//    {
//        //判断接收的数据是否有两字节，也就是文件的大小信息
//        //如果有则保存到blockSize变量中，没有则返回，继续接收数据
//        if(tcpSocket->bytesAvailable() < (int)sizeof(quint16))
//            return;
//        in >> blockSize;
//    }

//    if(tcpSocket->bytesAvailable() < blockSize) return;
//    //如果没有得到全部的数据，则返回，继续接收数据
//    in>>DATA_EMENU;
//    in >>document;
//    QString qba=document.toStdString().c_str();
//    qDebug()<<qba;
//    //将接收到的数据存放到变量中
//    QJsonParseError error;
//    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
//if(error.error==QJsonParseError::NoError)
//{
//    if(jsonDocument.isObject())
//    {

//        QJsonObject obj=jsonDocument.object();
////            QJsonValue b_value=obj.take("b");

////            qDebug()<<a_value;
////            ui->messageLabel->setText(body_value.toString());
//         qDebug()<<"form:readMessage()-before LoginSuccess";
//         qDebug()<<"result information-:"<<Information::getInstance()->SERVER_LOGIN_SUCCEED;
//         qDebug()<<"DATA_EMENU-:"<<DATA_EMENU;

//            if(DATA_EMENU==Information::getInstance()->SERVER_LOGIN_SUCCEED){
//                qDebug()<<"form:readMessage()-after LoginSuccess";
//                QJsonValue USERID=obj.take("userid");
//                QJsonValue NAME=obj.take("name");
//                QJsonValue SIGN=obj.take("sign");
//                QJsonValue HEADIMAGE=obj.take("headimage");
//                Information::getInstance()->userid=USERID.toString();
//                Information::getInstance()->name=NAME.toString();
//                Information::getInstance()->sign=SIGN.toString();
//                Information::getInstance()->headimage=HEADIMAGE.toString();
//                qDebug()<<"fuck ye "<<Information::getInstance()->userid;
//                //在跳转界面的时候要解除绑定和重新绑定
//                disconnect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readMessage()));
//                Talk* t=new Talk(tcpSocket);
//                qDebug()<<"now:"<<tcpSocket;
//                setVisible(false);
////                t->init(byte_array);
//                t->show();
//            }/*else
////                ui->login->setText("login fail");
//        }*/

//    }
//   }
//}

void Form::turntotalk(){
    if(helper->login_success){
        setVisible(false);
    }
}
