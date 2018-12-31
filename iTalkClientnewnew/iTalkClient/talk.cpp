#include "talk.h"
#include "ui_talk.h"
#include<information.h>
#include<friendmsg.h>
#include<groupmsg.h>

Talk::Talk(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),
    ui(new Ui::Talk)
{
    qDebug()<<"now entering setup talk";
    ui->setupUi(this);
    i=0;
    //setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(true);
    ui->label->installEventFilter(this);
    ui->label_2->installEventFilter(this);
    ui->label_9->installEventFilter(this);
    ui->label_10->installEventFilter(this);
    ui->label_8->installEventFilter(this);


    QImage image;
    image.load(":/myicon_3.png");
    QImage result = image.scaled(40, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_4->setPixmap(QPixmap::fromImage(result));

    QImage image2;
    image2.load(":/kobe_1.jpg");
    QImage result2 = image2.scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_3->setPixmap(QPixmap::fromImage(result2));
    ui->label_3->setStyleSheet("QLabel{padding:2px;border-radius:4px;}QLabel:hover{border:2px solid rgb(12, 255, 235);}");

    QImage image3;
    image3.load(":/myicon_1.png");
    //QImage result3 = image3.scaled(255, 511, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //QPalette palette(ui->widget->palette());
    //palette.setBrush(ui->widget->backgroundRole(), QBrush(result3));
    //ui->widget->setAutoFillBackground(true);
    //ui->widget->setPalette(palette);
    //ui->widget->show();
    //ui->widget->setWindowOpacity(0.7);
    qstacklayout=new QStackedLayout;
    ui->widget_2->setLayout(qstacklayout);
    //One* one=new One;


    drwer=new Drawer(this);
    drwer->handletcpsocket(tcpSocket);
    qDebug()<<"now start fucking press talk";
    two=new Two(tcpSocket);
    qDebug()<<"now start fucking press two";
    sunshine=new Sunshine;
    qstacklayout->addWidget(drwer);
    qstacklayout->addWidget(two);
    qstacklayout->addWidget(sunshine);
    QRect rect = QApplication::desktop()->availableGeometry();

    move(rect.width() - width()-280,5);
    QIcon icon = QIcon(":/kobe_1.jpg"); //png,ico,svg,bmp各种格式，尺寸无限制（推荐60x60），加载图标可以用相对、绝对、资源路径
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->show();
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                   this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pShowTimer = new QTimer(this);
    m_pShowTimer2 = new QTimer(this);
    connect(m_pShowTimer, SIGNAL(timeout()), this, SLOT(onMove()));
    connect(m_pShowTimer2, SIGNAL(timeout()), this, SLOT(onMove2()));
    //setMouseTracking(true);
    mouse_press=false;
    ui->label_8->installEventFilter(this);
    ui->label_9->installEventFilter(this);
    ui->label_10->installEventFilter(this);
    ui->label_6->setText(Information::getInstance()->name);
    ui->label_7->setText(Information::getInstance()->sign);

    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(showResearch()));
    //按回车键进入搜索
    //与server端建立连接
    //在此处进行服务器端的连接

    qDebug()<<"调用了talk连接";
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(solvedisconnect()));

    //通过轮训查找是否有人来添加好友或者群组，看是否愿意被添加成功，群租的话允许某人进入是不会在客户端
    //出现情况的，所以不用轮训查找，但是成功加入群组或者成功添加好友都会直接显示，所以这里还需要
    //轮训添加好友成功和添加群组成功的情况
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(agree_to_addfriend()));
    timer->start(1000);

    QTimer::singleShot(500,this,SLOT(initfriend_group()));






}

Talk::~Talk()
{
    delete ui;
}
void Talk::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<event->screenPos();
    if(event->button() == Qt::LeftButton)
     {
         this->setMouseTracking(true);
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void Talk::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
        mouse_press = false;
    if(y()<=0)
    {
        m_nDesktopHeight=0;
        m_pShowTimer->start(0.5);
    }
}

void Talk::mouseMoveEvent(QMouseEvent *event)
{
   ;
        if(mouse_press)
        {
            i++;
            //鼠标相对于屏幕的位置
            QPoint move_pos = event->globalPos();
           //移动主窗体位置
           this->move(move_pos - move_point);
        }
}

bool Talk::eventFilter(QObject *object, QEvent *e)
{
//    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(resolveMessage()));
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label){
        showMinimized();
        qDebug()<<"最小化";
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2){
         close();
         delete(this);
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_9){
        qDebug()<<"点击了第一页";
        qDebug()<<"talk resolvemessage槽触发";
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(resolveMessage()));
        qstacklayout->setCurrentIndex(0);
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_10){
        qDebug()<<"点击了第二页";
        qstacklayout->setCurrentIndex(1);
    }
    else if(e->type()==QEvent::MouseButtonPress&&object==ui->label_8){
        qDebug()<<"点击了第三页";
        qstacklayout->setCurrentIndex(2);
    }
    else if(object==ui->label_8&&e->type()==QEvent::Enter&&y()<0){
        qDebug()<<"鼠标进入了标签且y小于0";
         qDebug()<<e->type();
        //move(x(),5);
         m_nDesktopHeight2=y();
         m_pShowTimer2->start(0.5);

    }
    else if(object==ui->label_9&&e->type()==QEvent::Enter&&y()<0){
        qDebug()<<"鼠标进入了标签且y小于0";
        qDebug()<<e->type();
       //move(x(),5);
        m_nDesktopHeight2=y();
        m_pShowTimer2->start(0.5);
    }
    else if(object==ui->label_10&&e->type()==QEvent::Enter&&y()<0){
        qDebug()<<"鼠标进入了标签且y小于0";
         qDebug()<<e->type();
        //move(x(),5);
         m_nDesktopHeight2=y();
         m_pShowTimer2->start(0.5);
    }
    return false;
}
void Talk::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(3, 3, this->width()-6, this->height()-6);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillPath(path, QBrush(Qt::white));

        QColor color(0, 0, 0, 75);
        for(int i=0; i<3; i++)
        {
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            path.addRect(3-i, 3-i, this->width()-(3-i)*2, this->height()-(3-i)*2);
            color.setAlpha(50 - qSqrt(i)*25);
            painter.setPen(color);
            painter.drawPath(path);
        }
}
void Talk::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {

    case QSystemTrayIcon::DoubleClick:


        break;
    case QSystemTrayIcon::Trigger:

        //后面两个默认参数
        mouse_press=false;
        showNormal();
        raise();
        activateWindow();
        move(x(),5);
        break;
    case QSystemTrayIcon::MiddleClick:

        break;
    default:
        break;
    }
}
void Talk::hideEvent(QHideEvent *event)
{
    qDebug()<<"123";
    if(trayIcon->isVisible())
    {
        hide(); //隐藏窗口
        event->ignore(); //忽略事件
    }
}
void Talk::onMove()
{
    m_nDesktopHeight--;
    move(x(), m_nDesktopHeight);
    if (m_nDesktopHeight <= -610)
    {
        m_pShowTimer->stop();
        //hide();
    }
}
void Talk::onMove2()
{
    m_nDesktopHeight2++;
    move(x(), m_nDesktopHeight2);
    if (y() >=5)
    {
        m_pShowTimer2->stop();
        //hide();
    }
}
//查找好友
void Talk::showResearch()
{
    disconnect(tcpSocket,SIGNAL(readyRead()),this,SLOT(resolveMessage()));
    qDebug()<<"开始搜索";
    Search* s=new Search(tcpSocket);
    s->show();
    s->doSearch(ui->lineEdit->text());
}

//这个地方是登录之后，得到所有好友列表，同时得到所有之前的离线信息，还有在线的时候也会发来消息，发来的消息要在右下角显示出来
//接下来可以开始写注册了
//void Talk::resolveMessage()
//{
//    blockSize=0;
//    int DATA_EMENU=0;
//    QByteArray document;

//    qDebug()<<"readMessage()";
//    QDataStream in(tcpSocket);
//    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//    if(blockSize==0){
//        if(tcpSocket->bytesAvailable() < (int)sizeof(quint16))
//            return;
//        in >> blockSize;
//    }

//    if(tcpSocket->bytesAvailable() < blockSize) return;
//    in>>DATA_EMENU;
//    in >>document;
//    QString qba=document.toStdString().c_str();
//    qDebug()<<qba;
//    //将接收到的数据存放到变量中
//    QJsonParseError error;
//    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
//    if(error.error==QJsonParseError::NoError)
//    {
//        if(jsonDocument.isObject())
//        {

//            QJsonObject obj=jsonDocument.object();
//    //            QJsonValue b_value=obj.take("b");

//    //            qDebug()<<a_value;
//    //            ui->messageLabel->setText(body_value.toString());
//             qDebug()<<"form:readMessage()-before LoginSuccess";
//             qDebug()<<"result information-:"<<Information::getInstance()->SERVER_LOGIN_SUCCEED;
//             qDebug()<<"DATA_EMENU-:"<<DATA_EMENU;
//             if(DATA_EMENU==Information::getInstance()->CLIENT_ADD_FRIEND_PASSIVE){//显示被添加好友
//                 //提醒用户被添加了好友，所以要调用friendmsg的操作
//                 QJsonValue FRIENDID=obj.take("friendid");
//                 QJsonValue NAME=obj.take("name");
//                 QJsonValue SIGN=obj.take("sign");
//                 Friendmsg* friendmsg=new Friendmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),tcpSocket,this);
//                 friendmsg->show();
//             }
//             if(DATA_EMENU==Information::getInstance()->CLIENT_ADD_GROUP_PASSIVE){
//                 QJsonValue GROUPID=obj.take("groupid");
//                 QJsonValue NAME=obj.take("name");
//                 QJsonValue FRIENDID=obj.take("friendid");
//                 QJsonValue SIGN=obj.take("sign");
//                 Groupmsg* groupmsg=new Groupmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),GROUPID.toString(),tcpSocket,this);
//                 groupmsg->show();
//             }
//             //添加好友成功了,通知添加好友成功，更新客户端
//             //添加好友失败了,通知添加好友失败

//    }
//}

    //在线的时候，有人发送消息，直接显示发来的消息

//    QByteArray qba= tcpSocket->readAll(); //读取

//        QString ss=QVariant(qba).toString();
//        qDebug()<<ss;
//        QJsonParseError error;
//        QJsonDocument jsonDocument = QJsonDocument::fromJson(ss.toUtf8(), &error);
//        //QString ss=QVariant(qba).toString();
//        //qDebug("%s", qba);
//        //qDebug()<<ss;
//        if (error.error == QJsonParseError::NoError) {
//            if (jsonDocument.isObject()) {
//                /*QVariantMap result = jsonDocument.toVariant().toMap();
//                qDebug()<<result;
//                qDebug()<<result["userinfo"].toJsonArray();
//                foreach (QVariant plugin, result["userinfo"].toList()) {
//                            qDebug()<<plugin.toString();
//                   }*/
//                QJsonObject obj=jsonDocument.object();
//                //QJsonArray array=obj["userinfo"].toArray();
//                //这里是从服务器获得的信息
//                QJsonObject obj1=obj["usersInfoVo"].toObject();
//                qDebug()<<obj1["name"].toString();
//                //在这里进行输入和获取
//                ui->label_6->setText(Information::getInstance()->name);
//                ui->label_7->setText(Information::getInstance()->sign);
//                QJsonArray friends=obj1["friends"].toArray();
//                qDebug()<<friends;
//                for(int j=0;j<friends.size();j++){
//                    QJsonObject myfriend=friends[i].toObject();
//                    qDebug()<<myfriend["name"];
//                    QString name;
//                    if(myfriend["onLine"]==false)
//                        name=myfriend["name"].toString()+"[离线]";
//                    else
//                        name=myfriend["name"].toString()+"[在线]";
//                    drwer->addfriend(name,myfriend["sign"].toString());
//                }


//            }
//        } else {
//           qDebug()<<"解析失败";
//        }
       // socket->write(qba);
//}
//void Talk::init(QByteArray byte_array)
//{
//    qDebug()<<byte_array.append('\n');
//    qDebug()<<byte_array;
//    tcpSocket->write(byte_array);
//}
void Talk::solvedisconnect()
{
    qDebug()<<"socket连接断了";
}


void Talk::agree_to_addfriend(){
    //如果为true
    //同意被添加好友
    if(Information::getInstance()->agree_add_friend){
       this->drwer->addfriend(Information::getInstance()->add_friend_name,Information::getInstance()->add_friend_sign,Information::getInstance()->add_friend_id);
       Information::getInstance()->agree_add_friend=false;
       Information::getInstance()->add_friend_name="";
       Information::getInstance()->add_friend_sign="";
       Information::getInstance()->add_friend_id="";

    }

    //同意加入群组不需要进行ui操作

    //添加好友成功
    if(Information::getInstance()->add_friend_success){
        this->drwer->addfriend(Information::getInstance()->add_friend_name_other,Information::getInstance()->add_friend_sign_other,Information::getInstance()->add_friend_id_other);
        Information::getInstance()->add_friend_success=false;
        Information::getInstance()->add_friend_name_other="";
        Information::getInstance()->add_friend_sign_other="";
        Information::getInstance()->add_friend_id_other="";
    }

    //添加群组成功
    if(Information::getInstance()->add_group_success){
        this->two->additem(Information::getInstance()->add_group_name_other,Information::getInstance()->add_group_sign_other,Information::getInstance()->add_group_id_other);
        Information::getInstance()->add_group_success=false;
        Information::getInstance()->add_group_name_other="";
        Information::getInstance()->add_group_sign_other="";
        Information::getInstance()->add_group_id_other="";
    }

    //在主界面里面添加ui，创建群组成功
    if(Information::getInstance()->create_group_success){
        //进行ui更新
        this->two->additem(Information::getInstance()->create_group_name,Information::getInstance()->create_group_sign,Information::getInstance()->create_group_id);
        Information::getInstance()->create_group_success=false;
        Information::getInstance()->create_group_sign="";
        Information::getInstance()->create_group_name="";
        Information::getInstance()->create_group_id="";
    }

    //初始化朋友
//    if(!Information::getInstance()->groupidlist.empty()){
//        QMap<int,QString>::iterator it;
//        for(it=Information::getInstance()->groupidlist.begin();it!=Information::getInstance()->groupidlist.end();++it){
//            QString resultid=Information::getInstance()->groupidlist.take(it.key());
//            qDebug()<<"result:"<<resultid;
//            this->two->additem(Information::getInstance()->grouplist_id_name.take(resultid),
//                              Information::getInstance()->grouplist_id_sign.take(resultid)
//                               ,resultid);
//            Information::getInstance()->grouplist_id_name.remove(resultid);
//            Information::getInstance()->grouplist_id_sign.remove(resultid);
//            Information::getInstance()->groupidlist.remove(it.key());

//        }
//    }
//    if(Information::getInstance()->friendlist_show){
//       if(Information::getInstance()->friendlist_id_name.contains(Information::getInstance()->friendid)){
//          this->drwer->addfriend(Information::getInstance()->friendlist_id_name.take(Information::getInstance()->friendid),
//                             Information::getInstance()->friendlist_id_sign.take(Information::getInstance()->friendid),
//                             Information::getInstance()->friendid);

//          //插入好友
//          Information::getInstance()->friendid="";
//          Information::getInstance()->friendlist_id_name.remove(Information::getInstance()->friendid);
//          Information::getInstance()->friendlist_id_sign.remove(Information::getInstance()->friendid);
//          Information::getInstance()->friendlist_show=false;
//       }
//    }

    //初始化群组列表
//    if(Information::getInstance()->grouplist_show){
//       if(Information::getInstance()->grouplist_id_name.contains(Information::getInstance()->groupid)){
//          this->two->additem(Information::getInstance()->grouplist_id_name.take(Information::getInstance()->groupid),
//                             Information::getInstance()->grouplist_id_sign.take(Information::getInstance()->groupid),
//                             Information::getInstance()->groupid);

//          //插入好友
//          Information::getInstance()->groupid="";
//          Information::getInstance()->grouplist_id_name.remove(Information::getInstance()->groupid);
//          Information::getInstance()->grouplist_id_sign.remove(Information::getInstance()->groupid);
//          Information::getInstance()->grouplist_show=false;
//       }
//    }


    //只发送了作为群主的群，还要发送作为群成员的群

}

void Talk::initfriend_group(){
//    qDebug()<<"进入了这个函数";
    //take是取出啊！！！！value才是查看值啊！！！！
    if(!Information::getInstance()->groupidlist.empty()){
        QMap<int,QString>::iterator it;
        int i=0;
        for(it=Information::getInstance()->groupidlist.begin();it!=Information::getInstance()->groupidlist.end();++it){
            QString resultid=it.value();
            qDebug()<<"result:"<<resultid;
            qDebug()<<"i:"<<i;
            i++;
            this->two->additem(Information::getInstance()->grouplist_id_name.value(resultid),
                              Information::getInstance()->grouplist_id_sign.value(resultid)
                               ,resultid);
            qDebug()<<"i:"<<i;
//            Information::getInstance()->grouplist_id_name.remove(resultid);
//            Information::getInstance()->grouplist_id_sign.remove(resultid);
//            Information::getInstance()->groupidlist.remove(it.key());
            qDebug()<<"i:"<<i;

        }
    }

    if(!Information::getInstance()->friendidlist.empty()){
        QMap<int,QString>::iterator it;
        for(it=Information::getInstance()->friendidlist.begin();it!=Information::getInstance()->friendidlist.end();++it){
            QString resultid=it.value();
            this->drwer->addfriend(Information::getInstance()->friendlist_id_name.value(resultid),
                                   Information::getInstance()->friendlist_id_sign.value(resultid),
                                   resultid);
        }
    }
}

