#include "databaseoperation.h"

DataBaseOperation::DataBaseOperation()
{

}
QString DataBaseOperation::UserLogin(QString userID, QString userPsw)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    QVector<QString> groupIDVector;
    if(tempDatabase.open())
    {
        QJsonArray json_array;
        QJsonObject json;
        qDebug()<<"Login Database"<<userID<<"psw"<<userPsw;
        QString query=QString
                ("SELECT * FROM client_info WHERE Id='%1'").arg(userID);

        QSqlQuery registerQuery(query,tempDatabase);
        if(registerQuery.next())
        {
            QString account=registerQuery.value("Id").toString();
            QString name=registerQuery.value("Name").toString();
            QString sign=registerQuery.value("Sign").toString();
            QString password=registerQuery.value("Password").toString();
            QString headimage=registerQuery.value("Headimage").toString();

            //QString debug=account+name+sign+password+headimage;

            if(userPsw==password)
            {

                json.insert("loginresult","success");
                json.insert("userid",account);
                json.insert("name",name);
                json.insert("sign",sign);
                json.insert("headimage",headimage);
            }
            else
            {
                json.insert("loginresult","failed");
                json.insert("userid",account);
            }
        }
        else
        {
            json.insert("loginresult","user not found");
            json.insert("userid",userID);
        }
        //插入登陆结果与离线信息
        json_array.append(json);

        QString query_2=QString
                ("SELECT * FROM offline WHERE revieverID='%1'").arg(userID);

        QSqlQuery registerQuery_2(query_2,tempDatabase);
        while(registerQuery_2.next())
        {
            qDebug()<<"Login Database,find offline message:"<<userID;
            QJsonObject temp_json;
            QString recieverID=registerQuery_2.value("revieverID").toString();
            QString msgType=registerQuery_2.value("msgType").toString();
            QString msgJson=registerQuery_2.value("msgJson").toString();
            QString senderID=registerQuery_2.value("senderID").toString();
            QString respondType=registerQuery_2.value("respondType").toString();

            temp_json.insert("offlinemsg","offlinemsg");
            temp_json.insert("recieverid",recieverID);
            temp_json.insert("msgtype",msgType);
            temp_json.insert("msgjson",msgJson);
            temp_json.insert("senderid",senderID);
            temp_json.insert("respondtype",respondType);
            json_array.append(temp_json);

        }
        QString query_delete=QString("DELETE FROM offline WHERE revieverID='%1'").arg(userID);
        QSqlQuery registerQuery_delete(query_delete,tempDatabase);
        while(registerQuery_delete.next())
        {
            qDebug()<<QString("DELETE FROM offline WHERE revieverID='%1'").arg(userID);
        }

        //插入好友列表
        QString query_3=QString
                ("SELECT * FROM friend_list WHERE userID='%1'").arg(userID);

        QSqlQuery registerQuery_3(query_3,tempDatabase);
        while(registerQuery_3.next())
        {
            QString friendID=registerQuery_3.value("friendID").toString();
            qDebug()<<"Login Database,find Friend List:"<<friendID;

            QString query_friendinfo=QString("SELECT * FROM client_info WHERE Id='%1'").arg(friendID);
            QSqlQuery registerQuery_friendinfo(query_friendinfo,tempDatabase);
            if(registerQuery_friendinfo.next())
            {
                qDebug()<<"Login Database,find info Friend List:"<<userID;
                QString friendID_in=registerQuery_friendinfo.value("Id").toString();
                QString friendName=registerQuery_friendinfo.value("Name").toString();
                QString friendSign=registerQuery_friendinfo.value("Sign").toString();

                QJsonObject temp_json;
                temp_json.insert("friendlist","friendlist");
                temp_json.insert("friendid",friendID_in);
                temp_json.insert("friendname",friendName);
                temp_json.insert("friendsign",friendSign);
                json_array.append(temp_json);

            }
        }
        //插入群组列表
        QString query_4=QString
                ("SELECT * FROM group_member WHERE memberID='%1'").arg(userID);

        QSqlQuery registerQuery_4(query_4,tempDatabase);
        while(registerQuery_4.next())
        {

            qDebug()<<"Login Database,find Group List:"<<userID;
            QString groupID=registerQuery_4.value("groupID").toString();
            QString query_groupinfo=QString("SELECT * FROM group_info WHERE GroupID='%1'").arg(groupID);
            QSqlQuery registerQuery_groupinfo(query_groupinfo,tempDatabase);
            groupIDVector.append(groupID);
            if(registerQuery_groupinfo.next())
            {
                QString GroupID_in=registerQuery_groupinfo.value("GroupID").toString();
                QString GroupName=registerQuery_groupinfo.value("GroupName").toString();
                QString GroupIntroduce=registerQuery_groupinfo.value("GroupIntroduce").toString();
                QString GroupHolder=registerQuery_groupinfo.value("GroupHolder").toString();

                QJsonObject temp_json;
                temp_json.insert("grouplist","grouplist");
                temp_json.insert("groupid",GroupID_in);
                temp_json.insert("groupname",GroupName);
                temp_json.insert("groupintroduce",GroupIntroduce);
                temp_json.insert("groupholder",GroupHolder);
                json_array.append(temp_json);
            }
        }
        //插入群组成员
        qDebug()<<"DATABASE.插入群组成员,vector："<<groupIDVector;
        while(!groupIDVector.empty())
        {

            QString tempGroupID=groupIDVector.takeFirst();
            QString query_5=QString
                    ("SELECT memberID FROM group_member WHERE groupID='%1'").arg(tempGroupID);

            QSqlQuery registerQuery_5(query_5,tempDatabase);
            while(registerQuery_5.next())
            {

                qDebug()<<"Login Database,finding Group Member:"<<tempGroupID;
                QString memberID=registerQuery_5.value("memberID").toString();


                QString query_groupMember=QString("SELECT * FROM client_info WHERE Id='%1'").arg(memberID);

                QSqlQuery registerQuery_groupMember(query_groupMember,tempDatabase);
                if(registerQuery_groupMember.next())
                {
                    QString account=registerQuery_groupMember.value("Id").toString();
                    QString name=registerQuery_groupMember.value("Name").toString();
                    QString sign=registerQuery_groupMember.value("Sign").toString();
                    QString headimage=registerQuery_groupMember.value("Headimage").toString();

                    QJsonObject temp_json;

                    temp_json.insert("groupmember","groupmember");
                    temp_json.insert("groupid",tempGroupID);
                    temp_json.insert("userid",account);
                    temp_json.insert("name",name);
                    temp_json.insert("sign",sign);
                    temp_json.insert("headimage",headimage);

                    json_array.append(temp_json);

                }
            }
        }

        QJsonDocument document;
        document.setArray(json_array);
        QByteArray ba=document.toJson(QJsonDocument::Compact);
        QString json_str(ba);
        return json_str;
    }
    else
    {
        //QDebug()<<"DataBaseOperation.Login:数据库未打开";
        return "";
    }
}
QString DataBaseOperation::FriendSearch(QString friendID)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    QJsonObject resultJson;
    if(tempDatabase.open())
    {
        qDebug()<<"FriendSearch Database"<<friendID;
        QString query=QString
                ("SELECT * FROM client_info WHERE Id='%1'").arg(friendID);

        QSqlQuery registerQuery(query,tempDatabase);
        if(registerQuery.next())
        {
            QString account=registerQuery.value("Id").toString();
            QString name=registerQuery.value("Name").toString();
            QString sign=registerQuery.value("Sign").toString();
            QString headimage=registerQuery.value("Headimage").toString();

            //QString debug=account+name+sign+password+headimage;

            resultJson.insert("userid",account);
            resultJson.insert("name",name);
            resultJson.insert("sign",sign);
            resultJson.insert("headimage",headimage);

            QJsonDocument document;
            document.setObject(resultJson);
            QByteArray ba=document.toJson(QJsonDocument::Compact);
            QString json_str(ba);
            return json_str;

        }
        else
        {
            resultJson.insert("userid","");
            resultJson.insert("name","");
            resultJson.insert("sign","");
            resultJson.insert("headimage","");

            QJsonDocument document;
            document.setObject(resultJson);
            QByteArray ba=document.toJson(QJsonDocument::Compact);
            QString json_str(ba);
            return json_str;
        }
    }
    else
    {
        return "";
    }
}
QString DataBaseOperation::GroupSearch(QString groupID)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    QJsonObject resultJson;
    if(tempDatabase.open())
    {
        QString query=QString
                ("SELECT * FROM group_info WHERE GroupID='%1'").arg(groupID);

        QSqlQuery registerQuery(query,tempDatabase);
        if(registerQuery.next())
        {
            QString GroupID=registerQuery.value("GroupID").toString();
            QString GroupName=registerQuery.value("GroupName").toString();
            QString GroupIntroduce=registerQuery.value("GroupIntroduce").toString();
            QString GroupHolder=registerQuery.value("GroupHolder").toString();

            resultJson.insert("groupid",GroupID);
            resultJson.insert("groupname",GroupName);
            resultJson.insert("groupintroduce",GroupIntroduce);
            resultJson.insert("groupholder",GroupHolder);

            QJsonDocument document;
            document.setObject(resultJson);
            QByteArray ba=document.toJson(QJsonDocument::Compact);
            QString json_str(ba);
            return json_str;

        }
        else
        {
            resultJson.insert("groupid","");
            resultJson.insert("groupname","");
            resultJson.insert("groupintroduce","");
            resultJson.insert("groupholder","");

            QJsonDocument document;
            document.setObject(resultJson);
            QByteArray ba=document.toJson(QJsonDocument::Compact);
            QString json_str(ba);
            return json_str;
        }
    }
    else
    {
        return "";
    }
}

QString DataBaseOperation::GroupHolderSearch(QString groupID)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    QString groupHolderID="";
    if(tempDatabase.open())
    {
        QString query=QString
                ("SELECT GroupHolder FROM group_info WHERE GroupID='%1'").arg(groupID);

        QSqlQuery registerQuery(query,tempDatabase);
        if(registerQuery.next())
        {
            groupHolderID=registerQuery.value("GroupHolder").toString();
            return groupHolderID;

        }
        else
        {
            qDebug()<<"没找到群主？？？？？？";
            return groupHolderID;
        }
    }
    else
    {
        return "";
    }
}

QMap<QString,QMap<QString,QString>> DataBaseOperation::GroupInit()
{
    QMap<QString,QMap<QString,QString>> resultMap;
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    if(tempDatabase.open())
    {
        QString query=QString("SELECT GroupID FROM group_info");
        QSqlQuery registerQuery(query,tempDatabase);
        while(registerQuery.next())
        {
            QString GroupID=registerQuery.value("GroupID").toString();
            QMap<QString,QString> temp_groupMember;
            QString query_in=QString
                    ("SELECT memberID FROM group_member WHERE groupID='%1'").arg(GroupID);

            QSqlQuery registerQuery_in(query_in,tempDatabase);
            while(registerQuery_in.next())
            {
                QString memberID=registerQuery_in.value("memberID").toString();
                temp_groupMember.insert(memberID,memberID);
            }
            resultMap.insert(GroupID,temp_groupMember);
        }

    }
    return resultMap;
}

bool DataBaseOperation::AddOfflineChatMsg(QByteArray message)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    bool result=false;

    QString qba=message.toStdString().c_str();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
    QString recieverID="";
    QString bodyValue="";
    QString senderID="";
    QString bodyType="";
    QString respondType="";
    if(error.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isObject())
        {

            QJsonObject obj=jsonDocument.object();
            if(obj.contains("recieverid"))
            {
                recieverID=obj.take("recieverid").toString();
            }
            if(obj.contains("senderid"))
            {
                senderID=obj.take("senderid").toString();
            }
            if(obj.contains("bodyvalue"))
            {
                bodyValue=obj.take("bodyvalue").toString();
            }
            if(obj.contains("bodytype"))
            {
                bodyType=obj.take("bodytype").toString();
            }
            if(obj.contains("respondtype"))
            {
                respondType=obj.take("respondtype").toString();
            }
        }
        else
        {
            qDebug()<<"DATABASE.AddOfflineChatMsg:QJsonObject is not obj!";
        }
    }
    else
    {
        qDebug()<<"DATABASE.AddOfflineChatMsg:QJsonParseError:"<<error.error;
    }

    if(tempDatabase.open())
    {
        QString registerSql = QString("INSERT INTO offline (revieverID,msgType,msgJson,senderID,respondType) VALUES('%1','%2','%3','%4','%5')")
            .arg(recieverID)
            .arg(bodyType)
            .arg(bodyValue)
            .arg(senderID)
            .arg(respondType);
        QSqlQuery  query(tempDatabase);
        if (query.exec((registerSql)))
        {
            result=true;
        }
        else{
            qDebug()<<"DATABASE.AddOfflineChatMsg:插入数据库失败:"<<query.lastError();
            result=false;
        }
    }
    else
    {
        qDebug()<<"DATABASE.AddOfflineChatMsg:数据库未打开";
        result=false;
    }
    return result;

}

bool DataBaseOperation::UserRegister(QByteArray message)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    bool result=false;

    QString qba=message.toStdString().c_str();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
    QString nickname="";
    QString userID="";
    QString password="";
    QString headImage="";
    QString sign="";
    if(error.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isObject())
        {

            QJsonObject obj=jsonDocument.object();
            if(obj.contains("userid"))
            {
                userID=obj.take("userid").toString();
            }
            if(obj.contains("name"))
            {
                nickname=obj.take("name").toString();
            }
            if(obj.contains("password"))
            {
                password=obj.take("password").toString();
            }
            if(obj.contains("headimage"))
            {
                headImage=obj.take("headimage").toString();
            }
            if(obj.contains("sign"))
            {
                sign=obj.take("sign").toString();
            }
        }
        else
        {
            qDebug()<<"DATABASE.UserRegister:QJsonObject is not obj!";
        }
    }
    else
    {
        qDebug()<<"DATABASE.UserRegister:QJsonParseError:"<<error.error;
    }

    if(tempDatabase.open())
    {
        QString registerSql = QString("INSERT INTO client_info (Id,Password,Name,Sign,Headimage)"
            "VALUES('%1','%2','%3','%4','%5')")
            .arg(userID)
            .arg(password)
            .arg(nickname)
            .arg(sign)
            .arg(headImage);
        QSqlQuery  query(tempDatabase);
        if (query.exec((registerSql)))
        {
            result=true;
        }
        else{
            qDebug()<<"DATABASE.UserRegister:插入数据库失败，可能语句错误:"<<query.lastError();
            result=false;
        }
    }
    else
    {
        qDebug()<<"DATABASE.UserRegister:数据库未打开";
        result=false;
    }
    return result;
}

bool DataBaseOperation::GroupEstablish(QByteArray message)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    bool result=false;
    bool result1=false;

    QString qba=message.toStdString().c_str();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
    QString groupName="";
    QString groupID="";
    QString sign="";
    QString userID="";
    if(error.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isObject())
        {

            QJsonObject obj=jsonDocument.object();
            if(obj.contains("groupname"))
            {
                groupName=obj.take("groupname").toString();
            }
            if(obj.contains("groupid"))
            {
                groupID=obj.take("groupid").toString();
            }
            if(obj.contains("sign"))
            {
                sign=obj.take("sign").toString();
            }
            if(obj.contains("userid"))
            {
                userID=obj.take("userid").toString();
            }
        }
        else
        {
            qDebug()<<"DATABASE.GroupEstablish:QJsonObject is not obj!";
        }
    }
    else
    {
        qDebug()<<"DATABASE.GroupEstablish:QJsonParseError:"<<error.error;
    }

    if(tempDatabase.open())
    {
        QString registerSql = QString("INSERT INTO group_info (GroupID,GroupName,GroupIntroduce,GroupHolder)"
            "VALUES('%1','%2','%3','%4')")
            .arg(groupID)
            .arg(groupName)
            .arg(sign)
            .arg(userID);
        QSqlQuery  query(tempDatabase);
        if (query.exec((registerSql)))
        {
            result=true;
        }
        else{
            qDebug()<<"DATABASE.GroupEstablish:插入数据库失败，可能语句错误"<<query.lastError();
            result=false;
        }
        QString registerSql1 = QString("INSERT INTO group_member (memberID,groupID)"
            "VALUES('%1','%2')")
            .arg(userID)
            .arg(groupID);
        QSqlQuery  query1(tempDatabase);
        if (query1.exec((registerSql1)))
        {
            result1=true;
        }
        else{
            qDebug()<<"DATABASE.建群时群主插入数据库:插入数据库失败，可能语句错误"<<query1.lastError();
            result1=false;
        }
    }
    else
    {
        qDebug()<<"DATABASE.GroupEstablish:数据库未打开";
        result=false;
    }
    return result&&result1;
}





bool DataBaseOperation::friendListUpdate(QByteArray message)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    bool result=false;
    bool result1=false;
    QString qba=message.toStdString().c_str();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
    QString userID="";
    QString friendID="";
    if(error.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isObject())
        {

            QJsonObject obj=jsonDocument.object();
            if(obj.contains("userid"))
            {
                userID=obj.take("userid").toString();
            }
            if(obj.contains("friendid"))
            {
                friendID=obj.take("friendid").toString();
            }
        }
        else
        {
            qDebug()<<"DATABASE.friendListUpdate:QJsonObject is not obj!";
        }
    }
    else
    {
        qDebug()<<"DATABASE.friendListUpdate:QJsonParseError:"<<error.error;
    }

    if(tempDatabase.open())
    {
        QString registerSql = QString("INSERT INTO friend_list (userID,friendID)VALUES('%1','%2')")
            .arg(userID)
            .arg(friendID);
        QSqlQuery  query(tempDatabase);
        if (query.exec((registerSql)))
        {
            result=true;
        }
        else{
            qDebug()<<"DATABASE.friendListUpdate:插入数据库1失败，可能语句错误"<<query.lastError();
            result=false;
        }
        QString registerSql1 = QString("INSERT INTO friend_list (userID,friendID)"
            "VALUES('%1','%2')")
            .arg(friendID)
            .arg(userID);
        QSqlQuery  query1(tempDatabase);
        if (query1.exec((registerSql1)))
        {
            result1=true;
        }
        else{
            qDebug()<<"DATABASE.friendListUpdate:插入数据库2失败，可能语句错误"<<query1.lastError();
            result1=false;
        }
    }
    else
    {
        qDebug()<<"DATABASE.friendListUpdate:数据库未打开";
        result=false;
    }
    return result&result1;
}



bool DataBaseOperation::groupListUpdate(QByteArray message)
{
    QSqlDatabase tempDatabase = ServerConnectDatabase::getInstance();
    bool result=false;
    QString qba=message.toStdString().c_str();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
    QString userID="";
    QString groupID="";
    if(error.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isObject())
        {

            QJsonObject obj=jsonDocument.object();
            if(obj.contains("userid"))
            {
                userID=obj.take("userid").toString();
            }
            if(obj.contains("groupid"))
            {
                groupID=obj.take("groupid").toString();
            }
        }
        else
        {
            qDebug()<<"DATABASE.groupListUpdate:QJsonObject is not obj!";
        }
    }
    else
    {
        qDebug()<<"DATABASE.groupListUpdate:QJsonParseError:"<<error.error;
    }

    if(tempDatabase.open())
    {
        QString registerSql = QString("INSERT INTO group_member (memberID,groupID)VALUES('%1','%2')")
            .arg(userID)
            .arg(groupID);
        QSqlQuery  query(tempDatabase);
        if (query.exec((registerSql)))
        {
            result=true;
        }
        else{
            qDebug()<<"DATABASE.groupListUpdate:插入数据库失败，可能语句错误"<<query.lastError();
            result=false;
        }

    }
    else
    {
        qDebug()<<"DATABASE.groupListUpdate:数据库未打开";
        result=false;
    }
    return result;
}

DataBaseOperation:: ~DataBaseOperation()
{

}
