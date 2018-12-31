#include "server.h"
#include "ui_server.h"

server::server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::server)
{
    ui->setupUi(this);

    //构造函数：开启服务器
    tcpServer=new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,6666))
    {
        qDebug()<<"tcpServer Listen Fails!"<<tcpServer->errorString();
        close();
    }
    qDebug()<<"Server Started at port:6666";
    //构造函数：开始监听是否有新连接
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(openSession()));
    DataBaseOperation dataBaseOpertion;
    groupMap=dataBaseOpertion.GroupInit();
    qDebug()<<"groupMap Init:"<<groupMap;

    bytesReceived=0;
    fileNameSize=0;
    totalBytes=0;

}

void server::openSession()
{
    ui->textConnect->append(QString::fromUtf8("openSession：监听到新连接."));

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();//获取连接的Socket指针


    isOpen=false;

    //监听断开消息
    connect(clientConnection,SIGNAL(disconnected()),clientConnection,
            SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(disconnectDispose()));

    //监听被写消息，准备读写
    connect(clientConnection,SIGNAL(readyRead()),this,SLOT(readMessage()));

    connectNumber += 1;
    qDebug()<<QString::fromUtf8("openSession()：增加了一个连接，目前连接数:")<<connectNumber;

}

void server::disconnectDispose(){
    QTcpSocket * clientConnection = static_cast<QTcpSocket*>(sender());
    if (clientConnection->isOpen())
    {
        clientConnection->close();
    }
    for (int i = 0; i < socketVector.size();i++)
    {
         QMap<QString, QTcpSocket*> *childMap=socketVector.at(i);
         QMap<QString, QTcpSocket*>::iterator it; //遍历map
        for (it = childMap->begin(); it != childMap->end(); ++it)
        {
            if (!(it.value()->isOpen()))
            {
                socketVector.remove(i);
            }
        }
    }
    connectNumber -= 1;
    qDebug()<<QString::fromUtf8("disconnectDispose()：关闭了一个连接，目前连接数:")<<connectNumber;
    ui->textConnect->append("disconnectDispose：关闭了一个连接");
}

void server::readMessage()
{

    ui->textMessage->append("接收到一个请求，信息头为:");
    qDebug()<<"readMessage(),bytesReceived"<<bytesReceived;

    QTcpSocket * clientConnection=static_cast<QTcpSocket *>(sender());

    //quint16 blockSize=0;
    int DATA_MENU=0;

    QByteArray bytes;
    QByteArray document;
    QString ExtraParams;

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    if(bytesReceived<=sizeof(qint64)*2)
    {
        qDebug()<<"刚开始接收数据";
        if((clientConnection->bytesAvailable() >= sizeof(qint64)*2)&&(fileNameSize == 0))
        {
            qDebug()<<"接收数据总大小信息和文件名大小信息";
            in>>totalBytes>>fileNameSize;
            qDebug()<<"totalBytes"<<totalBytes<<",fileNameSize"<<fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }

        if((clientConnection->bytesAvailable() >= fileNameSize)
                &&(fileNameSize!=0))
        {
            qDebug()<<"接收文件名，并建立文件";
            in>>DATA_MENU>>ExtraParams;
            bytesReceived+=fileNameSize;

            QJsonParseError file_error;
            QJsonDocument file_document=
                    QJsonDocument::fromJson(ExtraParams.toUtf8(),&file_error);
            QString log="   ReadMessage解析请求,内容为：";
            log=log.append(ExtraParams);
            log=log.append(QString(DATA_MENU));
            ui->textMessage->append(log);
            qDebug()<<"ReadMessage解析请求:extraparams"<<ExtraParams;
            if(file_error.error==QJsonParseError::NoError)
            {
                if(file_document.isObject())
                {
                    qDebug()<<"ReadMessage:是Json Obj";
                    QJsonObject obj=file_document.object();
                    if(obj.contains("filename"))
                    {
                        filename=obj.take("filename").toString();
                        qDebug()<<"ReadMessage:是文件";
                        //filename=QString("D:/").append(filename);

                        qDebug()<<"接收文件名，并建立文件:"<<filename;
                        localFile=new QFile(filename);
                        if(!localFile->open(QFile::WriteOnly))
                        {
                            qDebug() << "open file error!";
                            return;
                        }
                        isOpen=true;
                        qDebug()<<"isOpen=true";
                    }
                }
                else
                {
                    qDebug()<<"ReadMessage:不是文件请求";
                }
            }

            qDebug()<<"DATA_MENU"<<DATA_MENU<<",ExtraParams"<<ExtraParams;
        }
        else
            return;

    }
    if(bytesReceived < totalBytes)
    {
        qDebug()<<"写入文件/Documents";
        bytesReceived += clientConnection->bytesAvailable() ;
        bytes=clientConnection->readAll();
        document=bytes;
        qDebug()<<"isOpen0"<<isOpen;
        if(isOpen)
        {
            qDebug()<<"isOpen1"<<isOpen;
            localFile->write(bytes);
        }
        qDebug()<<"isOpen1.5"<<isOpen;
        qDebug()<<"bytes size"<<bytes.size();
        //bytes.resize(0);
    }

    if(bytesReceived == totalBytes)

    { //接收数据完成时
        qDebug()<<"文件/消息接收成功:"<<bytes;
        QString log="文件/消息接收成功:";
        log=log.append(QString(bytes));
        ui->textMessage->append(log);

        qDebug()<<"isOpen2"<<isOpen;
        if(isOpen)
        {
            qDebug()<<"isOpen3"<<isOpen;
            localFile->close();
            bytesReceived=0;
            fileNameSize=0;
            totalBytes=0;
            isOpen=false;
        }
    }



//    QDataStream in(clientConnection);
//    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//    if(blockSize==0)
//    {
//        if(clientConnection->bytesAvailable() < (int)sizeof(quint16))
//            return;
//        in >> blockSize;
//    }
//    if(clientConnection->bytesAvailable() < blockSize) return;


//    in>>DATA_MENU;



    if(DATA_MENU==server::CLIENT_LOGIN)
    {
        QByteArray document=bytes;
        QString userID="";
        QString userPwd="";
        QString loginresult="";

        QJsonArray array;

        QJsonObject res_json1;
        bool haveQueryRes=false;

        //in >>document;
        QString qba=document.toStdString().c_str();

        qDebug()<<"Login:document"<<qba;
        QString log="   登陆请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
        if(error.error==QJsonParseError::NoError)
        {
            if(jsonDocument.isObject())
            {

                QJsonObject obj=jsonDocument.object();
                if(obj.contains("id"))
                {
                    userID=obj.take("id").toString();
                    log="   ";
                    log=log.append(userID);
                    log=log.append("--企图登陆");
                    ui->textMessage->append(log);
                }
                if(obj.contains("password"))
                {
                    userPwd=obj.take("password").toString();
                }
            }
            else
            {
                qDebug()<<"LOGIN:QJsonObject is not obj!";
                ui->textMessage->append("LOGIN:QJsonObject is not obj!");
            }
        }
        else
        {
            qDebug()<<"LOGIN:QJsonParseError:"<<error.error;
            ui->textMessage->append("LOGIN:QJsonParseError:");
        }

        //TODO:Check ID and Password
        DataBaseOperation dataBaseOpertion;
        QString dataMessage=dataBaseOpertion.UserLogin(userID,userPwd);

        QJsonParseError dataMessage_json_error;
        QJsonDocument dataMessage_parse_document=
                QJsonDocument::fromJson(dataMessage.toUtf8(),&dataMessage_json_error);
        if(dataMessage_json_error.error==QJsonParseError::NoError)
        {
            if(dataMessage_parse_document.isArray())
            {
                array=dataMessage_parse_document.array();

                for(int i=0;i<array.size();i++)
                {
                    QJsonObject temp_json=array.at(i).toObject();
                    if(temp_json.contains("loginresult"))
                    {
                        res_json1=temp_json;
                        haveQueryRes=true;
                    }
                }

                if(!haveQueryRes)
                    ui->textMessage->append("Database return no result");


                if(res_json1.contains("userid"))
                {
                    QString userid_temp=res_json1.take("userid").toString();
                    if(userid_temp!=userID)
                        qDebug()<<"UserID不匹配，却查询到了什么...";
                }

                if(res_json1.contains("loginresult"))
                {
                    loginresult=res_json1.take("loginresult").toString();
                }
            }
        }


        //Demo用：loginresult="success";//需要更改

        //res_json1.insert("loginresult",loginresult);


        if(loginresult=="success")//TODO:Login Success
        {
            QMap<QString, QTcpSocket*> *childMap = new QMap<QString, QTcpSocket*>();
            childMap->insert(userID, clientConnection);
            socketVector.push_back(childMap);
            qDebug() << userID << "-----" << clientConnection;
            log="   ";
            log=log.append(userID);
            log=log.append("--登陆成功");
            ui->textMessage->append(log);
        }

        qDebug()<<"Login return Message:"<<QString(dataMessage_parse_document.toJson());
        QByteArray messageSent;
        messageSent=dataMessage_parse_document.toJson(QJsonDocument::Compact);


        QByteArray block;
        qint64 totalbytes=messageSent.size();
        qint64 bytesToWrite=0;
        QString extra="";
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        out<<qint64 (0)<<qint64 (0);
        //Information::CLIENT_LOGIN<<extra;
        if(loginresult=="success")
            out<<server::SERVER_LOGIN_SUCCEED;
        else
            out<<server::SERVER_LOGIN_FAILED;
        out<<extra;

        totalbytes+=block.size();
        out.device()->seek(0);
        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
        bytesToWrite=totalbytes - clientConnection->write(block);
        block.resize(0);//清空缓存区
        block=messageSent;
        clientConnection->write(block);

//        QByteArray block;
//        QDataStream out(&block,QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//        out<<(quint16) 0;
//        if(loginresult=="success")
//            out<<server::SERVER_LOGIN_SUCCEED;
//        else
//            out<<server::SERVER_LOGIN_FAILED;

//        out<<messageSent;
//        out.device()->seek(0);
//        out<<(quint16) (block.size() - sizeof(quint16));

  //      //qDebug() << "Login res_json1"<<messageSent.toStdString().c_str() << "\n";
//        clientConnection->write(block);
        qDebug() << userID << "-----" << clientConnection;
        log="   ";
        log=log.append(userID);
        log=log.append("--登陆结果已发送:");
        log=log.append(QString(messageSent));
        ui->textMessage->append(log);

        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
        //TODO:读取数据库查看离线消息
    }
    //点对点聊天

    else if(DATA_MENU==server::CLIENT_REGISTER)
    {

        QByteArray req_byte=bytes;

        //in>>req_byte;
        QString qba=req_byte.toStdString().c_str();
        qDebug()<<"Register:document"<<qba;
        QString log="   用户注册请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        DataBaseOperation dataBaseOpertion;
        if(dataBaseOpertion.UserRegister(req_byte))
        {
            log="   ";
            log=log.append("--注册内容转存数据库成功");
        }
        else
        {
            log="   ";
            log=log.append("--注册内容转存数据库失败");
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_CHAT)
    {
        QString myuserID="";
        QString bodyValue="";
        QString friendID="";
        //TODO:添加消息头，表示bodyValue具体类型，图片或Text等，现在默认为text.
        QString bodyType="";
        QString name="";

        QJsonObject res_json;



        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"Login:document"<<qba;

        QString log="   点对点聊天请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
        if(error.error==QJsonParseError::NoError)
        {
            if(jsonDocument.isObject())
            {

                QJsonObject obj=jsonDocument.object();
                if(obj.contains("userid"))
                {
                    myuserID=obj.take("userid").toString();
                }
                if(obj.contains("friendid"))
                {
                    friendID=obj.take("friendid").toString();
                }
                if(obj.contains("bodyvalue"))
                {
                    bodyValue=obj.take("bodyvalue").toString();
                }
                if(obj.contains("bodytype"))
                {
                    bodyType=obj.take("bodytype").toString();
                }
                if(obj.contains("username"))
                {
                    name=obj.take("username").toString();
                }

            }
            else
            {
                qDebug()<<"CHAT:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"CHAT:QJsonParseError:"<<error.error;
        }

        res_json.insert("userid",friendID);
        res_json.insert("friendid",myuserID);
        res_json.insert("bodytype",bodyType);
        res_json.insert("bodyvalue",bodyValue);
        res_json.insert("friendname",name);


        QJsonObject offline_json;
        offline_json.insert("recieverid",friendID);
        offline_json.insert("senderid",myuserID);
        offline_json.insert("bodyvalue",bodyValue);
        offline_json.insert("bodytype",bodyType);
        offline_json.insert("respondtype","SERVER_CHAT_PASSIVE_RESPOND");



        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {


                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);



                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_CHAT_PASSIVE_RESPOND;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_CHAT_PASSIVE_RESPOND;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "Chat_Client res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(myuserID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--点对点发送成功");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            log="   ";
            log=log.append(myuserID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--点对点发送失败，转存数据库");
            ui->textMessage->append(log);
            //res_json.insert("respondtype","SERVER_CHAT_PASSIVE_RESPOND");
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);



            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }

        QJsonObject active_json;

        if(isSend)
            active_json.insert("result","对方在线，已发送");
        else
            active_json.insert("result","对方离线，数据库存储情况未知");

        QJsonDocument active_document;
        active_document.setObject(active_json);
        QByteArray activeSent;
        activeSent=active_document.toJson(QJsonDocument::Compact);



        QByteArray block;
        qint64 totalbytes=activeSent.size();
        qint64 bytesToWrite=0;
        QString extra="";
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        out<<qint64 (0)<<qint64 (0);
        //Information::CLIENT_LOGIN<<extra;
        out<<server::SERVER_CHAT_ACTIVE_RESPOND;
        out<<extra;

        totalbytes+=block.size();
        out.device()->seek(0);
        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
        bytesToWrite=totalbytes - clientConnection->write(block);
        block.resize(0);//清空缓存区
        block=activeSent;
        clientConnection->write(block);



//        QByteArray active_block;
//        QDataStream out(&active_block,QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//        out<<(quint16) 0;
//        out<<server::SERVER_CHAT_ACTIVE_RESPOND;
//        out<<activeSent;
//        out.device()->seek(0);
//        out<<(quint16) (active_block.size() - sizeof(quint16));
//        clientConnection->write(active_block);
        qDebug() << "Chat_Client_Active active_json"<<active_json << "\n";
        log="   ";
        log=log.append("已发送 信息情况 给主动发送人:");
        log=log.append(QString(activeSent));
        ui->textMessage->append(log);

        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_GROUP_CHAT_ACTIVE)
    {
        QString userID="";
        QString bodyValue="";
        QString groupID="";
        //TODO:添加消息头，表示bodyValue具体类型，图片或Text等，现在默认为text.
        QString bodyType="";
        QString name="";
        QString username="";

        //QMap<QString,QString> memberIDVector;
        QMap<QString,QMap<QString,QString>>::iterator idIt;

        QJsonObject res_json;
        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"CLIENT_GROUP_CHAT_ACTIVE:document"<<qba;

        QString log="   群组聊天请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("bodyvalue"))
                {
                    bodyValue=obj.take("bodyvalue").toString();
                }
                if(obj.contains("bodytype"))
                {
                    bodyType=obj.take("bodytype").toString();
                }
                if(obj.contains("groupname"))
                {
                    name=obj.take("groupname").toString();
                }
                if(obj.contains("username"))
                {
                    username=obj.take("username").toString();
                }

            }
            else
            {
                qDebug()<<"CLIENT_GROUP_CHAT_ACTIVE:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"CLIENT_GROUP_CHAT_ACTIVE:QJsonParseError:"<<error.error;
        }

        res_json.insert("userid",userID);
        res_json.insert("groupid",groupID);
        res_json.insert("bodytype",bodyType);
        res_json.insert("bodyvalue",bodyValue);
        res_json.insert("groupname",name);
        res_json.insert("friendname",username);

        bool isSend = false;



        //QMap<QString,QMap<QString,QString>>::iterator idIt;
        if(groupMap.contains(groupID))
        {
            idIt=groupMap.find(groupID);
        }
        else
        {
            qDebug()<<"没有找到输入的群组号";
            QString log="   没有找到输入的群组号:"+groupID;
            log=log.append(qba);
            ui->textMessage->append(log);
        }
        QMap<QString,QString> memberIDMap=idIt.value();


        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                //if (QString::compare(it.key(),friendID) ==0 )
                if(memberIDMap.contains(it.key())&&(QString::compare(it.key(),userID)!=0))
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);

                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_GROUP_CHAT_PASSIVE_RESPOND;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_GROUP_CHAT_PASSIVE_RESPOND;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "Group_Chat_Client res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(it.key());
                        log=log.append("--群聊发送成功");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_SEARCH_FRIEND)
    {
        QString userID;
        QString friendID;

        QByteArray document=bytes;

        QJsonObject res_json;

        //in >>document;
        QString qba=document.toStdString().c_str();

        qDebug()<<"Search friend:document"<<qba;
        QString log="   搜索好友请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                log="   ";
                log=log.append(userID);
                log=log.append("--企图搜索好友--");
                log=log.append(friendID);
                ui->textMessage->append(log);
            }
            else
            {
                qDebug()<<"SEARCHFRIEND:QJsonObject is not obj!";
                ui->textMessage->append("SEARCHFRIEND:QJsonObject is not obj!");
            }
        }
        else
        {
            qDebug()<<"SEARCHFRIEND:QJsonParseError:"<<error.error;
            ui->textMessage->append("SEARCHFRIEND:QJsonParseError");
        }

        DataBaseOperation dataBaseOpertion;
        QString dataMessage=dataBaseOpertion.FriendSearch(friendID);

        QJsonParseError dataMessage_json_error;
        QJsonDocument dataMessage_parse_document=
                QJsonDocument::fromJson(dataMessage.toUtf8(),&dataMessage_json_error);
        if(dataMessage_json_error.error==QJsonParseError::NoError)
        {
            qDebug()<<"SEARCHFRIEND:Res QJsonParseError:SUCCESS";
        }


        QByteArray messageSent;
        messageSent=dataMessage_parse_document.toJson(QJsonDocument::Compact);

        QByteArray block;
        qint64 totalbytes=messageSent.size();
        qint64 bytesToWrite=0;
        QString extra="";
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        out<<qint64 (0)<<qint64 (0);
        //Information::CLIENT_LOGIN<<extra;
        out<<server::SERVER_SEARCH_FRIEND_RESPOND;
        out<<extra;

        totalbytes+=block.size();
        out.device()->seek(0);
        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
        bytesToWrite=totalbytes - clientConnection->write(block);
        block.resize(0);//清空缓存区
        block=messageSent;
        clientConnection->write(block);



//        QByteArray block;
//        QDataStream out(&block,QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//        out<<(quint16) 0;
//        out<<server::SERVER_SEARCH_FRIEND_RESPOND;

//        out<<messageSent;
//        out.device()->seek(0);
//        out<<(quint16) (block.size() - sizeof(quint16));

        qDebug() << "SERCHFRIEND res_json"<<res_json << "\n";
        //clientConnection->write(block);
        qDebug() << userID << "-----" << clientConnection;
        log="   ";
        log=log.append(userID);
        log=log.append("--搜索好友结果已发送:");
        log=log.append(messageSent.toStdString().c_str());
        ui->textMessage->append(log);
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_SEARCH_GROUP)
    {
        QString userID;
        QString groupID;
        QByteArray document=bytes;

        QJsonObject res_json;

        //in >>document;
        QString qba=document.toStdString().c_str();

        qDebug()<<"Search Group:document"<<qba;
        QString log="   搜索群组请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                log="   ";
                log=log.append(userID);
                log=log.append("--企图搜索群组--");
                log=log.append(groupID);
                ui->textMessage->append(log);
            }
            else
            {
                qDebug()<<"Search Group:QJsonObject is not obj!";
                ui->textMessage->append("Search Group:QJsonObject is not obj!");
            }
        }
        else
        {
            qDebug()<<"SEARCHFRIEND:QJsonParseError:"<<error.error;
            ui->textMessage->append("Search Group:QJsonParseError:");
        }

        DataBaseOperation dataBaseOpertion;
        QString dataMessage=dataBaseOpertion.GroupSearch(groupID);

        QJsonParseError dataMessage_json_error;
        QJsonDocument dataMessage_parse_document=
                QJsonDocument::fromJson(dataMessage.toUtf8(),&dataMessage_json_error);
        if(dataMessage_json_error.error==QJsonParseError::NoError)
        {
            qDebug()<<"Search Group:Res QJsonParseError:SUCCESS";
        }


        QByteArray messageSent;
        messageSent=dataMessage_parse_document.toJson(QJsonDocument::Compact);



        QByteArray block;
        qint64 totalbytes=messageSent.size();
        qint64 bytesToWrite=0;
        QString extra="";
        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        out<<qint64 (0)<<qint64 (0);
        //Information::CLIENT_LOGIN<<extra;
        out<<server::SERVER_SEARCH_GROUP_RESPOND;
        out<<extra;

        totalbytes+=block.size();
        out.device()->seek(0);
        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
        bytesToWrite=totalbytes - clientConnection->write(block);
        block.resize(0);//清空缓存区
        block=messageSent;
        clientConnection->write(block);


//        QByteArray block;
//        QDataStream out(&block,QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//        out<<(quint16) 0;
//        out<<server::SERVER_SEARCH_GROUP_RESPOND;

//        out<<messageSent;
//        out.device()->seek(0);
//        out<<(quint16) (block.size() - sizeof(quint16));

        qDebug() << "Search Group res_json"<<res_json << "\n";
        //clientConnection->write(block);
        qDebug() << userID << "-----" << clientConnection;
        log="   ";
        log=log.append(userID);
        log=log.append("--搜索群组结果已发送:");
        log=log.append(messageSent.toStdString().c_str());
        ui->textMessage->append(log);
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_FRIEND)
    {
        QString userID="";
        QString friendID="";
        QString sign="";
        QString name="";


        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"Add_Freind:document"<<qba;

        QString log="   添加好友请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("sign"))
                {
                    sign=obj.take("sign").toString();
                }
                if(obj.contains("name"))
                {
                    name=obj.take("name").toString();
                }
            }
            else
            {
                qDebug()<<"ADD_FRIEND:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"ADD_FRIEND:QJsonParseError:"<<error.error;
        }

        res_json.insert("userid",friendID);
        res_json.insert("friendid",userID);
        res_json.insert("sign",sign);
        res_json.insert("name",name);


        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {


                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);

                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_FRIEND_PASSIVE;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);




//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_FRIEND_PASSIVE;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "AddFriend_Client res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--添加好友消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }

        QJsonObject offline_json;
        offline_json.insert("recieverid",friendID);
        offline_json.insert("senderid",userID);
        offline_json.insert("bodyvalue",name);
        offline_json.insert("bodytype",sign);
        offline_json.insert("respondtype","SERVER_ADD_FRIEND_PASSIVE");

        if(!isSend)
        {
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--添加好友消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_FRIEND_SUCCESS)
    {
        QString userID="";
        QString friendID="";
        QString sign="";
        QString name="";


        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"ADD_FRIEND_SUCCESS:document"<<qba;

        QString log="   添加好友通过请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("sign"))
                {
                    sign=obj.take("sign").toString();
                }
                if(obj.contains("name"))
                {
                    name=obj.take("name").toString();
                }
            }
            else
            {
                qDebug()<<"ADD_FRIEND_SUCCESS:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"ADD_FRIEND_SUCCESS:QJsonParseError:"<<error.error;
        }

        res_json.insert("userid",friendID);
        res_json.insert("friendid",userID);
        res_json.insert("sign",sign);
        res_json.insert("name",name);


        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);



                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_FRIEND_SUCCESS;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_FRIEND_SUCCESS;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "AddFriend_Client res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--添加好友通过消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",friendID);
            offline_json.insert("senderid",userID);
            offline_json.insert("bodyvalue",name);
            offline_json.insert("bodytype",sign);
            offline_json.insert("respondtype","SERVER_ADD_FRIEND_SUCCESS");
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--添加好友通过消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        QJsonDocument document_add;
        document_add.setObject(res_json);
        QByteArray databaseMsg;
        databaseMsg=document_add.toJson(QJsonDocument::Compact);
        DataBaseOperation dataBaseOpertion;
        if(dataBaseOpertion.friendListUpdate(databaseMsg))
        {
            log="   ";
            log=log.append("--好友数据库更新成功");
        }
        else
        {
            log="   ";
            log=log.append("--好友数据库更新失败");
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_FRIEND_FAIL)
    {
        QString userID="";
        QString friendID="";
        QString sign="";
        QString name="";


        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"CLIENT_ADD_FRIEND_FAIL:document"<<qba;

        QString log="   添加好友拒绝请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("sign"))
                {
                    sign=obj.take("sign").toString();
                }
                if(obj.contains("name"))
                {
                    name=obj.take("name").toString();
                }
            }
            else
            {
                qDebug()<<"CLIENT_ADD_FRIEND_FAIL:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"CLIENT_ADD_FRIEND_FAIL:QJsonParseError:"<<error.error;
        }

        res_json.insert("userid",friendID);
        res_json.insert("friendid",userID);
        res_json.insert("sign",sign);
        res_json.insert("name",name);


        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);



                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_FRIEND_FAIL;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);

//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_FRIEND_FAIL;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "ADD_FRIEND_FAIL res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--添加好友拒绝消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",friendID);
            offline_json.insert("senderid",userID);
            offline_json.insert("bodyvalue",name);
            offline_json.insert("bodytype",sign);
            offline_json.insert("respondtype","SERVER_ADD_FRIEND_FAIL");
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--添加好友拒绝消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_GROUP)
    {
        QString userID="";
        QString groupID="";
        QString sign="";
        QString name="";
        QString groupHolderID="";


        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"ADD_GROUP:document"<<qba;

        QString log="   添加群组请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("sign"))
                {
                    sign=obj.take("sign").toString();
                }
                if(obj.contains("name"))
                {
                    name=obj.take("name").toString();
                }
            }
            else
            {
                qDebug()<<"ADD_GROUP:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"ADD_GROUP:QJsonParseError:"<<error.error;
        }




        DataBaseOperation dataBaseOpertion;
        groupHolderID=dataBaseOpertion.GroupHolderSearch(groupID);

        res_json.insert("friendid",userID);
        res_json.insert("groupid",groupID);
        res_json.insert("sign",sign);
        res_json.insert("name",name);



        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),groupHolderID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的groupHolder，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);




                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_GROUP_PASSIVE;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_GROUP_PASSIVE;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "ADD_GROUP res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(groupHolderID);
                        log=log.append("--添加群组消息发送成功，群主在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",userID);
            offline_json.insert("senderid",groupID);
            offline_json.insert("bodyvalue",name);
            offline_json.insert("bodytype",sign);
            offline_json.insert("respondtype","SERVER_ADD_GROUP_PASSIVE");
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(groupHolderID);
            log=log.append("--添加群组消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_GROUP_SUCCESS)
    {
        QString userID="";
        QString groupID="";
        QString groupSign="";
        QString groupName="";
        QString friendID="";

        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"ADD_GROUP_SUCCESS:document"<<qba;

        QString log="   添加群组通过请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("friendid"))
                {
                    friendID=obj.take("friendid").toString();
                }
                if(obj.contains("groupname"))
                {
                    groupName=obj.take("groupname").toString();
                }
                if(obj.contains("sign"))
                {
                    groupSign=obj.take("sign").toString();
                }

            }
            else
            {
                qDebug()<<"ADD_GROUP_SUCCESS:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"ADD_GROUP_SUCCESS:QJsonParseError:"<<error.error;
        }

        DataBaseOperation dataBaseOpertionFindGroup;
        QString dataMessage=dataBaseOpertionFindGroup.GroupSearch(groupID);

        QJsonParseError findGroup_json_error;
        QJsonDocument findGroup_parse_document=
                QJsonDocument::fromJson(dataMessage.toUtf8(),&findGroup_json_error);
        if(findGroup_json_error.error==QJsonParseError::NoError)
        {

            if(findGroup_parse_document.isObject())
            {
                QJsonObject obj=findGroup_parse_document.object();
                if(obj.contains("groupintroduce"))
                {
                    groupSign=obj.take("groupintroduce").toString();
                }
                if(obj.contains("groupname"))
                {
                    groupName=obj.take("groupname").toString();
                }
            }
            else
            {
                qDebug()<<"ADD_GROUP_SUCCESS:findGroup_json is not obj!";
            }
        }

        res_json.insert("userid",friendID);
        res_json.insert("groupid",groupID);
        res_json.insert("groupsign",groupSign);
        res_json.insert("groupname",groupName);
        res_json.insert("friendid",userID);

        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);

                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_GROUP_SUCCESS;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_GROUP_SUCCESS;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "ADD_GROUP_SUCCESS res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--添加群组通过消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",userID);
            offline_json.insert("senderid",groupSign);
            offline_json.insert("bodyvalue",groupName);
            offline_json.insert("bodytype",groupID);
            offline_json.insert("respondtype","SERVER_ADD_GROUP_SUCCESS");
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--添加群组通过消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        QJsonObject update_json;
        update_json.insert("userid",friendID);
        update_json.insert("groupid",groupID);
        QJsonDocument document_update;
        document_update.setObject(update_json);
        QByteArray updateMsg;
        updateMsg=document_update.toJson(QJsonDocument::Compact);
        DataBaseOperation dataBaseOpertion;
        if(dataBaseOpertion.groupListUpdate(updateMsg))
        {
            log="   ";
            log=log.append("--群组数据库更新成功");
        }
        else
        {
            log="   ";
            log=log.append("--群组数据库更新失败");
        }

        QMap<QString,QMap<QString,QString>>::iterator it_groupMember=groupMap.find(groupID);
        //it_groupMember.value().contains(friendID);
        if(!it_groupMember.value().contains(friendID))
        {

//            QMap<QString,QString> temp_id_map;
//            temp_id_map.insert(friendID,friendID);
            it_groupMember.value().insert(friendID,friendID);
            qDebug()<<"服务器更新群组信息-加群成功，Map内容为："<<groupMap;
        }
        else
        {
            qDebug()<<"服务器更新群组信息-加群成功：该成员已存在！";
            QString log="   服务器更新群组信息-加群成功：该成员已存在！:"+friendID;
            log=log.append(qba);
            ui->textMessage->append(log);
        }

        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_ADD_GROUP_FAIL)
    {
        QString userID="";
        QString friendID="";
        QString groupID="";
        QString groupSign="";
        QString groupName="";

        QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"CLIENT_ADD_GROUP_FAIL:document"<<qba;

        QString log="   添加群组拒绝请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("groupid"))
                {
                    groupID=obj.take("groupid").toString();
                }
            }
            else
            {
                qDebug()<<"CLIENT_ADD_GROUP_FAIL:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"CLIENT_ADD_GROUP_FAIL:QJsonParseError:"<<error.error;
        }


        DataBaseOperation dataBaseOpertionFindGroup;
        QString dataMessage=dataBaseOpertionFindGroup.GroupSearch(groupID);

        QJsonParseError findGroup_json_error;
        QJsonDocument findGroup_parse_document=
                QJsonDocument::fromJson(dataMessage.toUtf8(),&findGroup_json_error);
        if(findGroup_json_error.error==QJsonParseError::NoError)
        {

            if(findGroup_parse_document.isObject())
            {
                QJsonObject obj=findGroup_parse_document.object();
                if(obj.contains("groupintroduce"))
                {
                    groupSign=obj.take("groupintroduce").toString();
                }
                if(obj.contains("groupname"))
                {
                    groupName=obj.take("groupname").toString();
                }
            }
            else
            {
                qDebug()<<"ADD_GROUP_FAIL:findGroup_json is not obj!";
            }
        }

        res_json.insert("userid",friendID);
        res_json.insert("groupid",groupID);
        res_json.insert("groupintroduce",groupSign);
        res_json.insert("groupname",groupName);
        res_json.insert("friendid",userID);


        bool isSend = false;

        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendID) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);


                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_ADD_GROUP_FAIL;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);


//                        QByteArray block;
//                        QDataStream out(&block,QIODevice::WriteOnly);
//                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//                        out<<(quint16) 0;
//                        out<<server::SERVER_ADD_GROUP_FAIL;
//                        out<<messageSent;
//                        out.device()->seek(0);
//                        out<<(quint16) (block.size() - sizeof(quint16));
                        qDebug() << "ADD_GROUP_FAIL res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userID);
                        log=log.append("-");
                        log=log.append(friendID);
                        log=log.append("--添加群组拒绝消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",friendID);
            offline_json.insert("senderid",userID);
            offline_json.insert("bodyvalue",groupName);
            offline_json.insert("bodytype",groupID);
            offline_json.insert("respondtype","SERVER_ADD_GROUP_FAIL");
            log="   ";
            log=log.append(userID);
            log=log.append("-");
            log=log.append(friendID);
            log=log.append("--添加群组拒绝消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_GROUP_ESTABLISH)
    {
        QString userID="";
        QString groupID="";
        QString groupSign="";
        QString groupName="";


        QByteArray req_byte=bytes;

        //in>>req_byte;
        QString qba=req_byte.toStdString().c_str();
        qDebug()<<"CLIENT_GROUP_ESTABLISH:document"<<qba;
        QString log="   用户建群请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);


        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
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
                if(obj.contains("groupname"))
                {
                    groupName=obj.take("groupname").toString();
                }
                if(obj.contains("sign"))
                {
                    groupSign=obj.take("sign").toString();
                }

            }
            else
            {
                qDebug()<<"GROUP_ESTABLISH:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"GROUP_ESTABLISH:QJsonParseError:"<<error.error;
        }


        DataBaseOperation dataBaseOpertion;
        if(dataBaseOpertion.GroupEstablish(req_byte))
        {
            log="   ";
            log=log.append("--建群内容转存数据库成功");
        }
        else
        {
            log="   ";
            log=log.append("--建群内容转存数据库失败");
        }


        if(!groupMap.contains(groupID))
        {

            QMap<QString,QString> temp_id_map;
            temp_id_map.insert(userID,userID);
            groupMap.insert(groupID,temp_id_map);
            qDebug()<<"服务器更新群组信息，Map内容为："<<groupMap;
        }
        else
        {
            qDebug()<<"服务器更新群组信息-创建：群组号已存在！";
            QString log="   服务器更新群组信息-创建：群组号已存在！"+groupID;
            log=log.append(qba);
            ui->textMessage->append(log);
        }
        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_CHAT_SEND_FILE)
    {
        QString filename="";
        QString userid="";
        QString friendid="";
        QString username="";
//        QString sign="";

        //QByteArray req_byte=bytes;
        QJsonParseError findGroup_json_error;
        QJsonDocument findGroup_parse_document=
                QJsonDocument::fromJson(ExtraParams.toUtf8(),&findGroup_json_error);
        QString log="   点对点文件发送请求,内容为：";
        log=log.append(ExtraParams);
        ui->textMessage->append(log);
        if(findGroup_json_error.error==QJsonParseError::NoError)
        {

            if(findGroup_parse_document.isObject())
            {
                QJsonObject obj=findGroup_parse_document.object();
                if(obj.contains("filename"))
                {
                    filename=obj.take("filename").toString();
                }
                if(obj.contains("userid"))
                {
                    userid=obj.take("userid").toString();
                }
                if(obj.contains("friendid"))
                {
                    friendid=obj.take("friendid").toString();
                }
                if(obj.contains("username"))
                {
                    username=obj.take("username").toString();
                }

            }
            else
            {
                qDebug()<<"CHAT_SEND_FILE:ExtraParams is not obj!";
            }
        }
//        QFile *localFile;
//        localFile = new QFile(filename);
//        if(!localFile->open(QFile::WriteOnly))
//        {
//            log="   文件打开失败";
//            ui->textMessage->append(log);
//            return;
//        }
//        localFile->write(bytes);
//        localFile->close();
        log="   文件保存成功：";
        qDebug()<<"文件保存成功";
        ui->textMessage->append(log);

        QJsonObject res_json;
        res_json.insert("userid",friendid);
        res_json.insert("friendid",userid);
        res_json.insert("filename",filename);
        res_json.insert("name",username);

        bool isSend = false;


        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if (QString::compare(it.key(),friendid) ==0 )
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);


                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_CHAT_FILE_PASSIVE;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);

                        qDebug() << "CHAT_FILE_SEND res_json"<<res_json << "\n";

                        //it.value()->write(block);
                        isSend = true;
                        log="   ";
                        log=log.append(userid);
                        log=log.append("-");
                        log=log.append(friendid);
                        log=log.append("--是否接受文件消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }
        if(!isSend)
        {
            QJsonObject offline_json;
            offline_json.insert("recieverid",friendid);
            offline_json.insert("senderid",userid);
            offline_json.insert("bodyvalue",filename);
            offline_json.insert("bodytype",username);
            offline_json.insert("respondtype","SERVER_CHAT_FILE_PASSIVE");

            log="   ";
            log=log.append(userid);
            log=log.append("-");
            log=log.append(friendid);
            log=log.append("--是否接受文件消息发送失败，转存数据库");
            ui->textMessage->append(log);
            QJsonDocument document;
            document.setObject(offline_json);
            QByteArray messageSent;
            messageSent=document.toJson(QJsonDocument::Compact);
            DataBaseOperation dataBaseOpertion;
            if(dataBaseOpertion.AddOfflineChatMsg(messageSent))
            {
                log="   ";
                log=log.append("--转存数据库成功");
            }
            else
            {
                log="   ";
                log=log.append("--转存数据库失败");
            }
        }
    }
    //bytes.resize(0);
    else if(DATA_MENU==server::CLIENT_CHAT_RECEIVE_FILE)
    {
        QString userID="";
        QString fileName="";


        //QJsonObject res_json;

        QByteArray req_document=bytes;
        //in>>req_document;
        QString qba=req_document.toStdString().c_str();
        qDebug()<<"CLIENT_CHAT_RECEIVE_FILE:document"<<qba;

        QString log="   接收文件请求,内容为：";
        log=log.append(qba);
        ui->textMessage->append(log);

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(qba.toUtf8(), &error);
        if(error.error==QJsonParseError::NoError)
        {
            if(jsonDocument.isObject())
            {

                QJsonObject obj=jsonDocument.object();
                if(obj.contains("userid"))
                {
                    userID=obj.take("userid").toString();
                }
                if(obj.contains("filename"))
                {
                    fileName=obj.take("filename").toString();
                }

            }
            else
            {
                qDebug()<<"CLIENT_CHAT_RECEIVE_FILE:QJsonObject is not obj!";
            }
        }
        else
        {
            qDebug()<<"CLIENT_CHAT_RECEIVE_FILE:QJsonParseError:"<<error.error;
        }

        QFile* fileSend=new QFile(filename);
        if(!fileSend->open(QFile::ReadOnly)){
            qDebug() << "CLIENT_CHAT_RECEIVE_FILE:open file error!";
            return;
        }
        QJsonDocument document_sendfile;
        QByteArray messageSentFile;
        QByteArray messageSentFile2;
        QByteArray block;

        QJsonObject json_file;
        json_file.insert("filename",fileName);
        json_file.insert("friendid",userID);

        document_sendfile.setObject(json_file);
        messageSentFile2=document_sendfile.toJson(QJsonDocument::Compact);
        QString extra=QString(messageSentFile2);
        qint64 totalBytesFile=fileSend->size();
        QDataStream fileout(&block,QIODevice::WriteOnly);

        fileout.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        fileout<<qint64 (0);
        fileout<<qint64 (0);

        fileout<<server::SERVER_CHAT_FILE_SEND;
        fileout<<extra;
        totalBytesFile += block.size();
        fileout.device()->seek(0);

        fileout<<totalBytesFile<<(qint64) (block.size() - sizeof(qint64)*2);
        clientConnection->write(block);
        qDebug()<<"服务器发送文件信息头完成 block："<<block;
        block.resize(0);
        messageSentFile=fileSend->readAll();
        block=messageSentFile;
        clientConnection->write(messageSentFile);
        fileSend->close();

        bytesReceived=0;
        fileNameSize=0;
        totalBytes=0;
    }

    else if(DATA_MENU==server::CLIENT_GROUP_CHAT_SEND_FILE)
    {
        QString filename="";
        QString userid="";
        QString groupid="";
        QString username="";
//        QString sign="";

        //QByteArray req_byte=bytes;
        QJsonParseError findGroup_json_error;
        QJsonDocument findGroup_parse_document=
                QJsonDocument::fromJson(ExtraParams.toUtf8(),&findGroup_json_error);
        QString log="   群聊文件发送请求,内容为：";
        log=log.append(ExtraParams);
        ui->textMessage->append(log);
        if(findGroup_json_error.error==QJsonParseError::NoError)
        {

            if(findGroup_parse_document.isObject())
            {
                QJsonObject obj=findGroup_parse_document.object();
                if(obj.contains("filename"))
                {
                    filename=obj.take("filename").toString();
                }
                if(obj.contains("userid"))
                {
                    userid=obj.take("userid").toString();
                }
                if(obj.contains("groupid"))
                {
                    groupid=obj.take("groupid").toString();
                }
                if(obj.contains("username"))
                {
                    username=obj.take("username").toString();
                }

            }
            else
            {
                qDebug()<<"CLIENT_GROUP_CHAT_SEND_FILE:ExtraParams is not obj!";
            }
        }
        log="   已经接收到了文件信息，准备开始接受，现在在CLIENT_GROUP_CHAT_SEND_FILE中";
        qDebug()<<"已经接收到了文件信息，准备开始接受，现在在CLIENT_GROUP_CHAT_SEND_FILE中";
        ui->textMessage->append(log);

        QJsonObject res_json;
        res_json.insert("userid",userid);
        res_json.insert("groupid",groupid);
        res_json.insert("filename",filename);
        res_json.insert("name",username);

        bool isSend = false;

        QMap<QString,QMap<QString,QString>>::iterator idIt;
        if(groupMap.contains(groupid))
        {
            idIt=groupMap.find(groupid);
        }
        else
        {
            qDebug()<<"file:没有找到输入的群组号";
            QString log="   file:没有找到输入的群组号:"+groupid;
            //log=log.append(qba);
            ui->textMessage->append(log);
        }
        QMap<QString,QString> memberIDMap=idIt.value();


        for (int i = 0; i < socketVector.size(); i++)
        {
            QMap<QString, QTcpSocket*> *childMap = socketVector.at(i);
            QMap<QString, QTcpSocket*>::iterator it; //遍历map
            for (it = childMap->begin(); it != childMap->end(); ++it)
            {

                if(memberIDMap.contains(it.key())&&(QString::compare(it.key(),userid)!=0))
                {
                    qDebug()<<"Socket连接中找到了在线的friend，开启状态位置，但有可能有问题";
                    if ((it.value()->isOpen()))
                    {
                        QJsonDocument document;
                        document.setObject(res_json);
                        QByteArray messageSent;
                        messageSent=document.toJson(QJsonDocument::Compact);

                        QByteArray block;
                        qint64 totalbytes=messageSent.size();
                        qint64 bytesToWrite=0;
                        QString extra="";
                        QDataStream out(&block,QIODevice::WriteOnly);//使用数据流写入数据
                        out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
                        out<<qint64 (0)<<qint64 (0);
                        //Information::CLIENT_LOGIN<<extra;
                        out<<server::SERVER_GROUP_CHAT_FILE_PASSIVE;
                        out<<extra;

                        totalbytes+=block.size();
                        out.device()->seek(0);
                        out<<totalbytes<<(qint64) (block.size() - sizeof(qint64)*2);
                        bytesToWrite=totalbytes - it.value()->write(block);
                        block.resize(0);//清空缓存区
                        block=messageSent;
                        it.value()->write(block);

                        qDebug() << "CLIENT_GROUP_CHAT_SEND_FILE res_json"<<res_json << "\n";

                        isSend = true;
                        log="   ";
                        log=log.append(userid);
                        log=log.append("-");
                        log=log.append(groupid);
                        log=log.append("--是否接受文件消息发送成功，对方在线");
                        ui->textMessage->append(log);
                        break;
                    }//用key()和data()分别获取“键”和“值”
                    else
                    {
                        socketVector.remove(i);
                    }
                }
            }
        }

    }
}


server::~server()
{
    delete ui;
}
QString server::getSocketUUID()
{
    QUuid uuid=QUuid::createUuid();
    return uuid.toString();
}
