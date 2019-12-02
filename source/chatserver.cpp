#include "chatserver.h"


chatServer::chatServer()
{
    tcpServer = new QTcpServer(); //наш класс - экземпляр класса QTcpServer
    connect(tcpServer, &QTcpServer::newConnection, this, &chatServer::newClient);

}

void chatServer::serverStart()
{
    connectToDB();
    if (tcpServer->listen(QHostAddress::Any, port)) {
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
    connect(newSession, &session::signal_newMessage, this, &chatServer::newMess);
    qDebug() << "New connect!!!\n";
    listSessions.push_back(newSession);
}

void chatServer::connectToDB()
{
    Database = QSqlDatabase::addDatabase("QSQLITE");
    Database.setDatabaseName ("//home/bart/Documents/GitHub/chat_IS_server/db/chat.db"); //("C:\\Users\\User\\Documents\\GitHub\\chat_IS_server\\db\\chat.db");

    if (!Database.open())
    {
        emit signal_dataBaseError(Database.lastError().text());
        qDebug() << Database.lastError().text();
    }
    else
    {
        qDebug() << "CONNECTED TO DB!!";
        emit signal_databaseOpen();
    }
}

void chatServer::chaingePort(quint16 port16)
{
    port = port16;
}

void chatServer::newMess(const QJsonDocument &doc)
{
    qDebug() << "doc to Message " << doc;
    int idRoom = doc.object()["idRoom"].toInt();

    foreach(session * thisSession, listSessions){
        if(thisSession->idThisRoom == idRoom){
            qDebug() << "Send message is look \n";
            thisSession->sendData(doc);
        }
        else{
            qDebug() << "Что то пошло не так!!!\n";
            doc.object()["message"] = "херово";
            thisSession->sendData(doc);
        }
    }
}
