#include "register.h"
#include "ui_register.h"
#include <QFileDialog>
#include<QNetworkRequest>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkReply>
#include<QJsonArray>
#include "register2.h"
#include "information.h"
Register::Register(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    i=0;
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    QMovie *movie = new QMovie(":/loginlogo.gif");
    ui->label->setMovie(movie);
    movie->start();
    ui->label_4->setStyleSheet("border-radius:15px;background-color:green;border-image:url(':/touxiang01.jpg')");
    ui->label_3->installEventFilter(this);//点击退出
    ui->label_5->installEventFilter(this);//点击修改头像
    ui->label_6->hide();
    
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
      
}

Register::~Register()
{
    delete ui;
}


bool Register::eventFilter(QObject* object,QEvent* e){
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_5){
        Register::openfile();//表示打开文件的操作
    }
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

//打开桌面文件的操作,点击之后设置头像
void Register::openfile(){
    fileName = QFileDialog::getOpenFileName(this);
    //这里获得的是标准路径
    qDebug()<<"filename:"<<fileName;
    qDebug()<<"\"";
    if(!fileName.isEmpty())
    {
        QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        qDebug()<<"currentfilename:"<<currentFileName;
        //这里可能存在问题
        //在这里显示选择之后的头像
        //存在问题，显示不出来
        QPixmap image;
        image.load(":/logo.jpg");
        if(!image){
            qDebug()<<"not empty";
        }
        ui->label_4->clear();
        ui->label_4->setPixmap(image);
        ui->label_4->show();//显示在这
        //图片还是没有显示出来，这个问题需要解决

        //在这里要显示头像并发送
    }
}

void Register::mouseMoveEvent(QMouseEvent *event)
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


void Register::mousePressEvent(QMouseEvent *event)
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

void Register::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Register::showEvent(QShowEvent *event){
    mouse_press=false;
}


void Register::on_pushButton_clicked(){
    qDebug()<<ui->lineEdit->text();
    if(ui->lineEdit->text()==""||ui->lineEdit_2->text()==""){
        qDebug()<<"内容为空";
        ui->label_6->setText("昵称或者密码不能为空");
    }
    if(ui->lineEdit_2->text()!=ui->lineEdit_3->text()){
         ui->label_6->show();
         ui->label_6->setText("两次密码不同");
         ui->lineEdit_2->setText("");
         ui->lineEdit_3->setText("");
    }else{
        qDebug()<<"进入了on_pushbutton";
//        Information::getInstance()->name=ui->lineEdit->text();
//        Information::getInstance()->password=ui->lineEdit_2->text();
//        qDebug()<<"name"<<Information::getInstance()->name;
        qDebug()<<"进入了on_pushbutton";
        Register2* r2=new Register2(ui->lineEdit->text(),ui->lineEdit_2->text(),tcpSocket);
        setVisible(false);
        r2->show();//跳转到注册的第二步去
    }
}
