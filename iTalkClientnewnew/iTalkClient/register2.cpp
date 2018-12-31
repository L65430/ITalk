#include "register2.h"
#include "ui_register2.h"
#include "register3.h"


Register2::Register2(QString name,QString password,QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),name(name),password(password),
    ui(new Ui::Register2)
{
    qDebug()<<"进入了register2";
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    QMovie *movie = new QMovie(":/loginlogo.gif");
    ui->label->setMovie(movie);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    movie->start();

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

Register2::~Register2()
{
    delete ui;
}

//点击
void Register2::on_pushButton_clicked(){
    Register3* r3=new Register3(name,password,ui->lineEdit->text(),tcpSocket);
    setVisible(false);
    r3->show();
}


bool Register2::eventFilter(QObject* object,QEvent* e){
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


void Register2::mouseMoveEvent(QMouseEvent *event)
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

void Register2::mousePressEvent(QMouseEvent *event)
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


void Register2::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void Register2::showEvent(QShowEvent *event){
    mouse_press=false;
}






