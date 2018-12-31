#include "creategroup.h"
#include "ui_creategroup.h"

CreateGroup::CreateGroup(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);i=0;
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

CreateGroup::~CreateGroup()
{
    delete ui;
}

void CreateGroup::on_pushButton_clicked(){//点击发送创建群组
    QJsonObject json;
    json.insert("userid",Information::getInstance()->userid);
    json.insert("groupname",ui->lineEdit->text());
    json.insert("groupid",ui->lineEdit_3->text());
    json.insert("sign",ui->lineEdit_2->text());
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
    out<<Information::CLIENT_GROUP_ESTABLISH;
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
    setVisible(false);
    //告诉主界面及时更新
    Information::getInstance()->create_group_success=true;
    Information::getInstance()->create_group_name=ui->lineEdit->text();
    Information::getInstance()->create_group_sign=ui->lineEdit_2->text();
    Information::getInstance()->create_group_id=ui->lineEdit_3->text();

}


bool CreateGroup::eventFilter(QObject* object,QEvent* e){
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

void CreateGroup::mouseMoveEvent(QMouseEvent *event)
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

void CreateGroup::mousePressEvent(QMouseEvent *event)
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


void CreateGroup::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;

}

void CreateGroup::showEvent(QShowEvent *event){
    mouse_press=false;
}



