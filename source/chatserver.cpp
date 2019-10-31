#include "chatserver.h"


chatServer::chatServer()
{
    tcpServer = new QTcpServer(); //наш класс - экземпляр класса QTcpServer
    connect(tcpServer, &QTcpServer::newConnection, this, &chatServer::newClient);
}

void chatServer::serverStart()
{
    connectToDB();
    if (tcpServer->listen(QHostAddress::Any, 5000)) {
        emit signal_serverStarted();
    }
    else {
        qDebug() << "Не удалось открыть сетевой интерфейс"
                 << tcpServer->errorString();
        emit signal_serverError(tcpServer->errorString());
    }    
}

void chatServer::newClient()
{
    QTcpSocket * socket = tcpServer->nextPendingConnection();

    session * newSession = new session(socket);
    qDebug() << "New connect!!!\n";
    listSessions.push_back(newSession);
}

void chatServer::connectToDB()
{
    Database = QSqlDatabase::addDatabase("QSQLITE");
    Database.setDatabaseName("//home/bart/Документы/GitHub/chat_IS/db/chat.db");  //("C:\\Users\\User\\Documents\\chat_IS\\db\\chat.db");

    if (!Database.open())
    {
        qDebug() << Database.lastError().text();
    }
    else
    {
        qDebug() << "CONNECTED TO DB!!";
    }
}
