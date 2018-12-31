#ifndef SERVERCONNECTDATABASE_H
#define SERVERCONNECTDATABASE_H
#include <QtSql>

class ServerConnectDatabase
{
private:
    static QSqlDatabase qDatabase;
public:
    ServerConnectDatabase();
    ~ServerConnectDatabase();

    static QSqlDatabase &getInstance();
};

#endif // SERVERCONNECTDATABASE_H
