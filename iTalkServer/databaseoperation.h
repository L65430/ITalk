#ifndef DATABASEOPERATION_H
#define DATABASEOPERATION_H

#include <QSqlDatabase>
#include <QObject>
#include "serverconnectdatabase.h"

class DataBaseOperation :public QObject
{
public:
    DataBaseOperation();

    QString UserLogin(QString userID,QString userPsw);

    QString FriendSearch(QString friendID);

    QString GroupSearch(QString groupID);

    QString GroupHolderSearch(QString groupID);

    QMap<QString,QMap<QString,QString>> GroupInit();


    bool AddOfflineChatMsg(QByteArray message);

    bool UserRegister(QByteArray message);

    bool GroupEstablish(QByteArray message);

    bool friendListUpdate(QByteArray message);

    bool groupListUpdate(QByteArray message);

    //TODO:有新功能继续加
    //QString LoginReturn(QString userID,QString userPsw);
    ~DataBaseOperation();

};

#endif // DATABASEOPERATION_H
