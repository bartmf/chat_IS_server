#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QTcpServer>
#include <QByteArray>
#include <QSqlDatabase>
#include <QList>
#include <QDebug>
#include <QTcpSocket>
#include <QDataStream>
#include <QSqlError>
#include "session.h"




class chatServer: public QObject
{
    Q_OBJECT
public:
    QList <session*> listSessions;
    chatServer();
    void serverStart();
    void newClient();
    void newData();
    void connectToDB();

private:
    QTcpServer* tcpServer; //наш класс от класса QTcpServer
    QSqlDatabase Database;


signals:
    void signal_serverError(const QString&);
    void signal_serverStarted();
    void newDataAvaible(const QByteArray&);

};

#endif // CHATSERVER_H
