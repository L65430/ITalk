#ifndef SERVER_H
#define SERVER_H
#include <QWidget>
#include <QtNetwork>
#include "serverconnectdatabase.h"
#include "databaseoperation.h"
namespace Ui {
class server;
}

class server : public QWidget
{
    Q_OBJECT

public:
    explicit server(QWidget *parent = 0);
    ~server();

private:
    Ui::server *ui;

    //服务器指针
    QTcpServer *tcpServer;

    //存放 键为userID，值为SocketConnection指针的映射Map 的列表
    QVector<QMap<QString, QTcpSocket*>*> socketVector;

    QMap<QString,QMap<QString,QString>> groupMap;

    QFile *localFile;

    qint64 bytesReceived;
    qint64 fileNameSize;
    qint64 totalBytes;
    QString filename;


    bool isOpen;

    //服务器当前连接数
    qint8 connectNumber=0;

    //发送客户端UUID以进行服务器验证
    QString sendClientUUID(QString &uuid);

    //获取Socket UUID
    inline QString getSocketUUID( );

    //消息头
    //前2位 客户端or服务器 客户端请求：10 服务器处理：11
    //中2位 请求类型 登录：01 聊天：02 搜索好友/群：03
    //后2位 序号
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

        CLIENT_GROUP_CHAT_ACTIVE=33,

        CLIENT_CHAT_SEND_FILE=34,SERVER_CHAT_FILE_PASSIVE=35,//前者直接发文件给服务器，后者服务器发消息告诉客户端是否接收

        CLIENT_CHAT_RECEIVE_FILE=36,SERVER_CHAT_FILE_SEND=37,//前者发消息告诉服务器要接收，后者服务器直接发文件给用户

        CLIENT_GROUP_CHAT_SEND_FILE=38,SERVER_GROUP_CHAT_FILE_PASSIVE=39,
    };
private slots:
    //客户端开启会话时调用
    void openSession();

    //客户端对发起请求时调用
    void readMessage();

    //客户端关闭会话时调用
    void disconnectDispose();
};

#endif // SERVER_H
