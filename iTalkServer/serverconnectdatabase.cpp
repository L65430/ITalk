#include "serverconnectdatabase.h"

QSqlDatabase ServerConnectDatabase::qDatabase;

ServerConnectDatabase::ServerConnectDatabase()
{
    ServerConnectDatabase::getInstance();
}

QSqlDatabase & ServerConnectDatabase::getInstance()
{
    if(ServerConnectDatabase::qDatabase.open())
    {
        return ServerConnectDatabase::qDatabase;
    }
    else
    {
        ServerConnectDatabase::qDatabase=QSqlDatabase::addDatabase("QMYSQL","CHAT");
        //设置相关参数
        ServerConnectDatabase::qDatabase.setDatabaseName("italk_server");    // 需要与数据库中的一致
        ServerConnectDatabase::qDatabase.setHostName("127.0.0.1");    // 主机名(地址)
        ServerConnectDatabase::qDatabase.setPort(3306);        // 端口
        ServerConnectDatabase::qDatabase.setUserName("root");  //设置用户名
        ServerConnectDatabase::qDatabase.setPassword("GAMElab403.");  //设置密码
        if (ServerConnectDatabase::qDatabase.open())
        {
            qDebug() << "open sucess";
            return ServerConnectDatabase::qDatabase;
        }
        else
        {
            qDebug() << "open failed";
            return ServerConnectDatabase::qDatabase;
        }
    }
}
ServerConnectDatabase::~ServerConnectDatabase()
{

}

