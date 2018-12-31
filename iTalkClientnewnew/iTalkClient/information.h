#ifndef INFORMATION_H
#define INFORMATION_H
#include <QWidget>
#include<QLabel>
#include<QMovie>
#include<QMouseEvent>
#include<QDebug>
#include<QPropertyAnimation>
#include<QImage>
#include<QDesktopServices>
#include<QPropertyAnimation>
#include<QNetworkAccessManager>
#include<QCryptographicHash>
#include<QVector>
#include<QMap>
class Information{
public:
    QString userid;
    QString password;
    QString name;
    QString sign;
    QString headimage;
    enum{

            CLIENT_LOGIN = 1 ,

            SERVER_LOGIN_SUCCEED = 2,SERVER_LOGIN_FAILED = 3,

            CLIENT_REGISTER = 4,

            CLIENT_SEARCH_FRIEND = 5,CLIENT_SEARCH_GROUP = 6,

            SERVER_SEARCH_FRIEND_RESPOND = 7,SERVER_SEARCH_GROUP_RESPOND=8,

            CLIENT_ADD_FRIEND = 9,SERVER_ADD_FRIEND_PASSIVE = 10,

            CLIENT_ADD_FRIEND_PASSIVE=11,SERVER_ADD_FRIEND_ACTIVE = 12,

            CLIENT_ADD_GROUP = 13,SERVER_ADD_GROUP_PASSIVE = 14,

            CLIENT_ADD_GROUP_PASSIVE=15,SERVER_ADD_GROUP_ACTIVE = 16,

            CLIENT_GROUP_ESTABLISH = 17,

            CLIENT_CHAT_INIT=18,CLIENT_CHAT=19,

            SERVER_CHAT_INIT_RESPOND=20,SERVER_CHAT_PASSIVE_RESPOND=21,SERVER_CHAT_ACTIVE_RESPOND=22,

            SERVER_GROUP_CHAT_INIT_RESPOND=23,SERVER_GROUP_CHAT_PASSIVE_RESPOND=24,

            //这里是新增的信息头，用下面的
            CLIENT_ADD_FRIEND_SUCCESS=25,CLIENT_ADD_FRIEND_FAIL=26,//添加好友成功和失败

            CLIENT_ADD_GROUP_SUCCESS=27,CLIENT_ADD_GROUP_FAIL=28,// 后者是表示被动接收的用户，处理在线和离线的，在线的就发过去，离线就存在这个用户的离线信息里

            SERVER_ADD_FRIEND_SUCCESS=29,SERVER_ADD_FRIEND_FAIL=30,

            SERVER_ADD_GROUP_SUCCESS=31,SERVER_ADD_GROUP_FAIL=32,

            CLIENT_GROUP_CHAT_ACTIVE=33,CLIENT_CHAT_SEND_FILE=34,//用户发送信息

            SERVER_CHAT_FILE_PASSIVE=35,CLIENT_CHAT_RECEIVE_FILE=35,//同意接受

            SERVER_CHAT_FILE_SEND=36,






        };


    static Information* getInstance();
    static Information* in_p;

    QString search_group_name;
    QString search_friend_name;//在搜索过程中改变的ui

    //同意添加好友
    bool agree_add_friend=false;
    QString add_friend_name;
    QString add_friend_sign;
    QString add_friend_id;


    //同意添加群组
    bool agree_add_group=false;
    QString add_group_name;
    QString add_group_sign;
    QString add_group_id;


    //添加他人成功
    bool add_friend_success=false;
    QString add_friend_name_other;
    QString add_friend_sign_other;
    QString add_friend_id_other;

    //添加群组成功
    bool add_group_success=false;
    QString add_group_name_other;
    QString add_group_sign_other;
    QString add_group_id_other;

    //创建群组成功
    bool create_group_success=false;
    QString create_group_name;
    QString create_group_sign;
    QString create_group_id;


    //判断点对点是否正在聊天
    QVector<QString> talklist;

    QMap<QString,QString> talkmap;




    //判断群组是否正在聊天
    QVector<QString> grouplist;
    QMap<QString,QString> groupmap;


    //好友列表相关部分
    int friendidnumber=0;
    QMap<int,QString> friendidlist;
    QMap<QString,QString> friendlist_id_name;
    QMap<QString,QString> friendlist_id_sign;


    //群组列表相关部分
    int groupidnumebr=0;
    QMap<int,QString> groupidlist;
    QMap<QString,QString> grouplist_id_name;
    QMap<QString,QString> grouplist_id_sign;
    QMap<QString,QString> groupid_userid;

   //群组具体信息
    //用来存群组id,群员id,群员身份
    QMap<QString,QString> memberid_memberstate;
    QMap<QString,QMap<QString,QString>> groupid_memberid_mermberstate;
    //用来存群组id,群员id，群员名字
    QMap<QString,QString> memberid_membername;
    QMap<QString,QMap<QString,QString>> groupid_memberid_membername;




};

#endif // INFORMATION_H
