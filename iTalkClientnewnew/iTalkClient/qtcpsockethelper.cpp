#include "qtcpsockethelper.h"
#include "ui_qtcpsockethelper.h"
#include<QNetworkRequest>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkReply>
#include<QJsonArray>
#include "form.h"
#include "information.h"
#include "talk.h"
#include "groupmsg.h"
#include "friendmsg.h"
#include "mymessagebox.h"
#include "mylistwidget.h"
#include"filenotify.h"
QTcpSocketHelper::QTcpSocketHelper(QTcpSocket* tcpSocket,QWidget *parent) :
    QWidget(parent),tcpSocket(tcpSocket),//在这里进行处理
    ui(new Ui::QTcpSocketHelper)
{
    ui->setupUi(this);
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(resolvemessage()));
}

QTcpSocketHelper::~QTcpSocketHelper()
{
    delete ui;
}

void QTcpSocketHelper::resolvemessage(){

    //文件处理的部分
    qint64 totalBytes=0;
    qint64 bytesReceived=0;
    qint64 fileNameSize=0;
    QString ExtraParams;





    blockSize=0;
    qDebug()<<"form readmessage槽触发";
   // Information*resultinformation=Information::getInstance();
    int DATA_EMENU=0;
    QByteArray document;
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    //设置数据流版本，这里要和服务器端相同



    if(bytesReceived<=sizeof(qint64)*2)
        {
            if((tcpSocket->bytesAvailable() >= sizeof(qint64)*2)&&(fileNameSize == 0))
            {
                in>>totalBytes>>fileNameSize;
                bytesReceived+=sizeof(qint64)*2;
            }

            if((tcpSocket->bytesAvailable() >= fileNameSize)&&(fileNameSize!=0))
            {
                in>>DATA_EMENU;
                in>>ExtraParams;//如果是文件发送，是一个jsonobject，如果不是就是空
                bytesReceived+=fileNameSize;
            }
            else
                return;

        }
        if(bytesReceived < totalBytes)
        {
            bytesReceived+=tcpSocket->bytesAvailable() ;
            document=tcpSocket->readAll();
        }






    //如果没有得到全部的数据，则返回，继续接收数
    qDebug()<<"DATA_EMENU"<<DATA_EMENU;
    QString qba=document.toStdString().c_str();
    qDebug()<<qba;
    //将接收到的数据存放到变量中
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);

    //登录成功
     if(DATA_EMENU==Information::getInstance()->SERVER_LOGIN_SUCCEED){
         QJsonArray qja=jsonDocument.array();
         for(int i=0;i<qja.size();i++){
             QJsonObject obj=qja.at(i).toObject();
             if(obj.contains("loginresult")){
                 QJsonValue USERID=obj.take("userid");
                 QJsonValue NAME=obj.take("name");
                 QJsonValue SIGN=obj.take("sign");
                 QJsonValue HEADIMAGE=obj.take("headimage");
                 Information::getInstance()->userid=USERID.toString();
                 Information::getInstance()->name=NAME.toString();
                 Information::getInstance()->sign=SIGN.toString();
                 Information::getInstance()->headimage=HEADIMAGE.toString();
                 qDebug()<<"fuck ye "<<Information::getInstance()->userid;
                 Talk* t=new Talk(tcpSocket);
                 qDebug()<<"now:"<<tcpSocket;
                 setVisible(false);
       //                t->init(byte_array);
                 t->show();
                 login_success=true;
             }
            if(obj.contains("offlinemsg")){
                qDebug()<<"接收到一个离线消息："<<obj;
               QJsonValue RESPONDTYPE=obj.take("respondtype");
               //TODO
               //上线告诉你有人要加你
              if(RESPONDTYPE.toString()=="SERVER_ADD_FRIEND_PASSIVE"){
                  //上线的时候有人要添加你
                  //消息要显示出来
                  QJsonValue FRIENDID=obj.take("senderid");
                  QJsonValue NAME=obj.take("msgjson");
                  QJsonValue SIGN=obj.take("msgtype");
                  Friendmsg* friendmsg=new Friendmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),tcpSocket);
                  friendmsg->show();

              }

              //上线告诉你添加别人成功
              if(RESPONDTYPE.toString()=="SERVER_ADD_FRIEND_SUCCESS"){
                  QJsonValue FRIENDID=obj.take("senderid");
                  QJsonValue USERID=obj.take("recievered");//这是取出的结果
                  QJsonValue NAME=obj.take("msgjson");
                  QJsonValue SIGN=obj.take("msgtype");
                  //还有头像，要考虑
                  Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                  mylistwidget->init(FRIENDID.toString(),"添加成功",NAME.toString(),2);
                  //name和发过来的话的话,就是这两个
                  Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                  mymessagebox->setname(Information::getInstance()->name);
                  mymessagebox->show();
                  //同时添加到information里面
                  Information::getInstance()->add_friend_success=true;
                  Information::getInstance()->add_friend_name_other=NAME.toString();
                  Information::getInstance()->add_friend_sign_other=SIGN.toString();
                  Information::getInstance()->add_friend_id_other=FRIENDID.toString();//这是说当前这个添加成功

              }

              if(RESPONDTYPE.toString()=="SERVER_ADD_FRIEND_FAIL"){
                  QJsonValue FRIENDID=obj.take("senderid");
                  QJsonValue USERID=obj.take("recievered");//这是取出的结果
                  QJsonValue NAME=obj.take("msgjson");
                  QJsonValue SIGN=obj.take("msgtype");
                  //还有头像，要考虑
                  Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                  mylistwidget->init(FRIENDID.toString(),"添加失败",NAME.toString(),2);//添加失败就不做处理了
                  Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                  mymessagebox->setname(Information::getInstance()->name);
                  mymessagebox->show();

              }

              if(RESPONDTYPE.toString()=="SERVER_ADD_GROUP_PASSIVE")
              {
                  QJsonValue GROUPID=obj.take("senderid");
                  QJsonValue NAME=obj.take("msgjson");
                  QJsonValue FRIENDID=obj.take("recieverid");
                  QJsonValue SIGN=obj.take("msgtype");
                  Groupmsg* groupmsg=new Groupmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),GROUPID.toString(),tcpSocket);
                  groupmsg->show();

              }

              if(RESPONDTYPE.toString()=="SERVER_ADD_GROUP_SUCCESS"){
                  //QJsonValue  FRIENDID=obj.take("senderid");
                  QJsonValue  GROUPNAME=obj.take("msgjson");
                  QJsonValue  GROUPSIGN=obj.take("senderid");
                  //这里需要groupid传回来
                  QJsonValue  GROUPID=obj.take("msgtype");
                  Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                  mylistwidget->init(GROUPID.toString(),"添加成功",GROUPNAME.toString(),2);
                  Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                  mymessagebox->setname(Information::getInstance()->name);
                  mymessagebox->show();
                  //然后通知群组成功，在talk里面更新ui
                  Information::getInstance()->add_group_success=true;
                  Information::getInstance()->add_group_name_other=GROUPNAME.toString();
                  Information::getInstance()->add_group_sign_other=GROUPSIGN.toString();
                  Information::getInstance()->add_group_id_other=GROUPID.toString();//这里还需要ID传进来


              }


              if(RESPONDTYPE.toString()=="SERVER_ADD_GROUP_FAIL"){
                  QJsonValue GROUPID=obj.take("msgtype");
                  QJsonValue GROUPNAME=obj.take("msgjson");
                  Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                  mylistwidget->init(GROUPID.toString(),"添加失败",GROUPNAME.toString(),2);//添加失败就不做处理了
                  Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                  mymessagebox->setname(Information::getInstance()->name);
                  mymessagebox->show();

              }

              //这里点对点同样需要设置那个东西，就是点击的时候可以自动跳转
              if(RESPONDTYPE.toString()=="SERVER_CHAT_PASSIVE_RESPOND"){
                QJsonValue BODYTYPE=obj.take("msgtype");
                if(BODYTYPE.toString()=="text"){
                    //如果是群组的话
                    QJsonValue  FRIENDID=obj.take("senderid");
                    //QJsonValue  BODTTYPE=obj.take("msgtype");
                    QJsonValue  BODYVALUE=obj.take("msgjson");
                    QJsonValue  FRIENDNAME=obj.take("recieverid");
                    Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                    mylistwidget->init(FRIENDNAME.toString(),BODYVALUE.toString(),FRIENDID.toString(),1);
                    Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                    mymessagebox->setname(Information::getInstance()->name);
                    mymessagebox->show();

                }
              }

              //群聊

           }

            if(obj.contains("friendlist")){
                QJsonValue FRIENDID=obj.take("friendid");
                QJsonValue FRIENDNAME=obj.take("friendname");
                QJsonValue FRIENDSIGN=obj.take("friendsign");
                QString resultid=FRIENDID.toString();
                QString resultname=FRIENDNAME.toString();
                QString resultsign=FRIENDSIGN.toString();
                //把结果插入talk列表
                //也需要轮训获取结果并插入

                //drwer需要改动
//               Information::getInstance()->friendlist_show=true;
               Information::getInstance()->friendidlist.insert(Information::getInstance()->friendidnumber,resultid);
               Information::getInstance()->friendidnumber++;
               Information::getInstance()->friendlist_id_name.insert(resultid,resultname);
               Information::getInstance()->friendlist_id_sign.insert(resultid,resultsign);
               //这里要插进去
            }

            //群组列表
            if(obj.contains("grouplist")){
               QJsonValue GROUPID=obj.take("groupid");
               QJsonValue GROUPNAME=obj.take("groupname");
               QJsonValue GROUPINTRODUCE=obj.take("groupintroduce");
               QJsonValue GROUPHOLDER=obj.take("groupholder");
               QString resultid=GROUPID.toString();
               QString resultname=GROUPNAME.toString();
               QString resultintroduce=GROUPINTRODUCE.toString();
               QString resultholder=GROUPHOLDER.toString();

               //然后把群组信息存入最终结果中去
               //holder信息也要存到里面去
//               Information::getInstance()->grouplist_show=true;
               Information::getInstance()->groupidlist.insert(Information::getInstance()->groupidnumebr,resultid);
//               qDebug()<<"插入结果为："<<Information::getInstance()->groupidlist.take(Information::getInstance()->groupidnumebr);
               Information::getInstance()->groupidnumebr++;
               Information::getInstance()->grouplist_id_name.insert(resultid,resultname);
               Information::getInstance()->grouplist_id_sign.insert(resultid,resultintroduce);

               //存入holder信息
               Information::getInstance()->memberid_memberstate.insert(resultholder,"builder");
               qDebug()<<Information::getInstance()->memberid_memberstate.value(resultholder);
               Information::getInstance()->groupid_memberid_mermberstate.insert(resultid,Information::getInstance()->memberid_memberstate);



            }


            //包括群组成员
            if(obj.contains("groupmember")){
               QJsonValue GROUPID=obj.take("groupid");
               QJsonValue USERID=obj.take("userid");
               QJsonValue NAME=obj.take("name");//这是群组name
               QString resultgroupid=GROUPID.toString();
               QString resultuserid=USERID.toString();
               QString resultname=NAME.toString();


               if(!Information::getInstance()->memberid_memberstate.contains(resultuserid)){
                   Information::getInstance()->memberid_memberstate.insert(resultuserid,"member");
               }
                   Information::getInstance()->memberid_membername.insert(resultuserid,resultname);
                   Information::getInstance()->groupid_memberid_membername.insert(resultgroupid,Information::getInstance()->memberid_membername);
                   Information::getInstance()->groupid_memberid_mermberstate.insert(resultgroupid,Information::getInstance()->memberid_memberstate);
            }


        }
     }

    //搜索界面，得到了搜索结果之后要返回到ui显示
     //原来显示好的ui要在这个时候能够改变
     //搜索朋友操作
     if(DATA_EMENU==Information::getInstance()->SERVER_SEARCH_FRIEND_RESPOND){
         QJsonObject obj=jsonDocument.object();
         QJsonValue USERNAME=obj.take("name");
         Information::getInstance()->search_friend_name=USERNAME.toString();
     }

     //搜索群组结果
     if(DATA_EMENU==Information::getInstance()->SERVER_SEARCH_GROUP_RESPOND){
         QJsonObject obj=jsonDocument.object();
         QJsonValue USERNAME=obj.take("groupname");
         Information::getInstance()->search_group_name=USERNAME.toString();
     }

     //显示被添加好友
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_FRIEND_PASSIVE){
          QJsonObject obj=jsonDocument.object();
          QJsonValue FRIENDID=obj.take("friendid");
          QJsonValue NAME=obj.take("name");
          QJsonValue SIGN=obj.take("sign");
          Friendmsg* friendmsg=new Friendmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),tcpSocket);
          friendmsg->show();
      }

      //显示有人要加入群组
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_GROUP_PASSIVE){
          //就是这里，这里显示出来
          QJsonObject obj=jsonDocument.object();
          QJsonValue GROUPID=obj.take("groupid");
          QJsonValue NAME=obj.take("name");
          QJsonValue FRIENDID=obj.take("friendid");
          QJsonValue SIGN=obj.take("sign");
          Groupmsg* groupmsg=new Groupmsg(NAME.toString(),FRIENDID.toString(),SIGN.toString(),GROUPID.toString(),tcpSocket);
          groupmsg->show();
      }

      //添加好友成功
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_FRIEND_SUCCESS){
         //在下面显示已经添加成功
         //然后更新ui，就是直接在里面用
         QJsonObject obj=jsonDocument.object();
         QJsonValue FRIENDID=obj.take("friendid");
         QJsonValue USERID=obj.take("userid");//这是取出的结果
         QJsonValue NAME=obj.take("name");
         QJsonValue SIGN=obj.take("sign");
         //还有头像，要考虑
         Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
         mylistwidget->init(FRIENDID.toString(),"添加成功",NAME.toString(),2);
         //name和发过来的话的话,就是这两个
         Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
         mymessagebox->setname(Information::getInstance()->name);
         mymessagebox->show();
         //同时添加到information里面
         Information::getInstance()->add_friend_success=true;
         Information::getInstance()->add_friend_name_other=NAME.toString();
         Information::getInstance()->add_friend_sign_other=SIGN.toString();
         Information::getInstance()->add_friend_id_other=FRIENDID.toString();//这是说当前这个添加成功
         //要放在talk中轮训

      }

      //添加好友失败，写在下面，不更新ui
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_FRIEND_FAIL){
           QJsonObject obj=jsonDocument.object();
           QJsonValue FRIENDID=obj.take("friendid");
           QJsonValue USERID=obj.take("userid");//这是取出的结果
           QJsonValue NAME=obj.take("name");
           QJsonValue SIGN=obj.take("sign");
           //还有头像，要考虑
           Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
           mylistwidget->init(FRIENDID.toString(),"添加失败",NAME.toString(),2);//添加失败就不做处理了
           Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
           mymessagebox->setname(Information::getInstance()->name);
           mymessagebox->show();
      }

      //添加群组成功,更新ui
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_GROUP_SUCCESS){
           //传到里面去然后增加
           //还有建立群组成功也要写,就是还要写建群
           QJsonObject obj=jsonDocument.object();
           QJsonValue  FRIENDID=obj.take("friendid");
           QJsonValue  GROUPNAME=obj.take("groupname");
           QJsonValue  GROUPSIGN=obj.take("groupsign");
           //这里需要groupid传回来
           QJsonValue  GROUPID=obj.take("groupid");
           Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
           mylistwidget->init(GROUPID.toString(),"添加成功",GROUPNAME.toString(),2);
           Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
           mymessagebox->setname(Information::getInstance()->name);
           mymessagebox->show();
           //然后通知群组成功，在talk里面更新ui
           Information::getInstance()->add_group_success=true;
           Information::getInstance()->add_group_name_other=GROUPNAME.toString();
           Information::getInstance()->add_group_sign_other=GROUPSIGN.toString();
           Information::getInstance()->add_group_id_other=GROUPID.toString();//这里还需要ID传进来
           //这里可能没有写

      }

      //添加群组失败
      if(DATA_EMENU==Information::getInstance()->SERVER_ADD_GROUP_FAIL){
         QJsonObject obj=jsonDocument.object();
         QJsonValue GROUPID=obj.take("groupid");
         QJsonValue GROUPNAME=obj.take("groupname");
         Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
         mylistwidget->init(GROUPID.toString(),"添加失败",GROUPNAME.toString(),2);//添加失败就不做处理了
         Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
         mymessagebox->setname(Information::getInstance()->name);
         mymessagebox->show();

      }

      //如果是照片的话
      //接收到消息，如果在聊的话，就直接显示，如果目前不是和对方在聊的话，还要在右下角显示一下
      if(DATA_EMENU==Information::getInstance()->SERVER_CHAT_PASSIVE_RESPOND){
        bool flag=false;//显示是不是正在聊天
        QJsonObject obj=jsonDocument.object();
        QJsonValue  FRIENDID=obj.take("friendid");
        QJsonValue  BODTTYPE=obj.take("bodytype");
        QJsonValue  BODYVALUE=obj.take("bodyvalue");
        QJsonValue  FRIENDNAME=obj.take("friendname");
        //如果是在当前界面聊天的话，直接显示在界面上，如果不是的话，还需要在右下角显示出来，告诉他有人在找
        //如果是直接文字消息的话，就直接显示
        if(BODTTYPE.toString()=="text"){
            for(int i=0;i<Information::getInstance()->talklist.size();i++){
               if(Information::getInstance()->talklist.at(i)==FRIENDID.toString()){//是在聊天的过程中
                  //直接显示在聊天界面上（聊天界面那个部分需要轮询查找），
                  //需要在这里建立一个东西，用来显示可以即时显示在界面上的东西，在聊天界面需要进行轮询
                   Information::getInstance()->talkmap.insert(FRIENDID.toString(),BODYVALUE.toString());//把一一对应结果存进去               
                   flag=true;//表示在其中
                   break;
               }
           }
            //表示不在其中，通过在主界面的轮询提醒用户有人发来消息
            if(flag==false){
                int isperson=1;
                Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                mylistwidget->init(FRIENDNAME.toString(),BODYVALUE.toString(),FRIENDID.toString(),isperson);
                Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                mymessagebox->setname(Information::getInstance()->name);
                mymessagebox->show();
                //右下角弹框点击之后进入聊天界面
            }
        }




      }

      //还需要对接一下，这个地方的消息头的发送
      if(DATA_EMENU==Information::getInstance()->SERVER_GROUP_CHAT_PASSIVE_RESPOND){
           bool flag=false;//显示是不是正在群聊
           QJsonObject obj=jsonDocument.object();
           QJsonValue  GROUPID=obj.take("groupid");
           QJsonValue  BODTTYPE=obj.take("bodytype");
           QJsonValue  BODYVALUE=obj.take("bodyvalue");
           QJsonValue  GROUPNAME=obj.take("groupname");
           QJsonValue  FRIENDNAME=obj.take("friendname");
           if(BODTTYPE.toString()=="text"){
               for(int i=0;i<Information::getInstance()->grouplist.size();i++){
                  if(Information::getInstance()->grouplist.at(i)==GROUPID.toString()){//是在聊天的过程中
                     //直接显示在聊天界面上（聊天界面那个部分需要轮询查找），
                     //需要在这里建立一个东西，用来显示可以即时显示在界面上的东西，在聊天界面需要进行轮询
                      Information::getInstance()->groupmap.insert(GROUPID.toString(),BODYVALUE.toString());//把一一对应结果存进去
                      flag=true;//表示在其中
                      break;
                  }
                  //在群组聊天界面进行轮询
              }
               if(flag==false){
                   int isgroup=0;
                   Mylistwidget* mylistwidget=new Mylistwidget(tcpSocket);
                   mylistwidget->init(GROUPNAME.toString(),BODYVALUE.toString(),GROUPID.toString(),isgroup);
                   Mymessagebox* mymessagebox=new Mymessagebox(mylistwidget);
                   mymessagebox->setname(Information::getInstance()->name);
                   mymessagebox->show();
               }

           }
      }


      //通知有人要发送文件，然后是接收或者拒绝
      if(DATA_EMENU==Information::getInstance()->SERVER_CHAT_FILE_PASSIVE){
          QJsonObject obj=jsonDocument.object();
          QJsonValue  GROUPID=obj.take("groupid");
          QJsonValue  BODTTYPE=obj.take("bodytype");
          QJsonValue  BODYVALUE=obj.take("bodyvalue");
          QJsonValue  GROUPNAME=obj.take("groupname");
          QJsonValue  FRIENDNAME=obj.take("friendname");
//          Filenotify *filenotify=new Filenotify()

      }


      //接受文件的过程，点击之后有一个接受的界面，点开
      if(DATA_EMENU==Information::getInstance()->SERVER_CHAT_FILE_SEND){

      }



      //相互添加，创建群组功能完成

      //获取群组信息



      //还有进行语音通话的功能，只支持点对点语音通话的功能







}


