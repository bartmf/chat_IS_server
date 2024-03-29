#ifndef SESSION_H
#define SESSION_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlError>
#include <QDataStream>
#include <QByteArray>
#include <QThread>
#include <QJsonArray>
#include <QMap>
#include <QVariantMap>

namespace commandCode {
    const int reg = 1;
    const int message = 2;
    const int roomCreated = 3;
    const int invite = 5;
    const int auth = 10;
    const int invalidAuth = 11;
    const int loginAlreadyUsed = 12;
    const int dataBaseError = 13;
    const int sendRoomId = 14;
    const int sendUserId = 15;
}

class session:public QObject
{
    Q_OBJECT



public:
    QTcpSocket * client;
    int idThisUser;
    int idThisRoom = 1;

public:
    session(QTcpSocket * client);
    void sendMessage(const QJsonDocument &doc);
    void sendData(const QJsonDocument &data);
private:
    void doCommand(const QJsonDocument &doc);
    void auth(const QJsonDocument &doc);
    void registration(const QJsonDocument &doc);
    void newData();
    void newMessag(const QJsonDocument &doc);
    QJsonObject sendRoom();
    QJsonObject SendRoomUsers(int idRoom = 1);
    void sendRoomOnClick (const QJsonDocument &doc);
    void creatRoom (const QJsonDocument &doc);
    void sendHistoryMessage();
    void sendInviteToRoom (const QJsonDocument &doc);



signals:
    void signal_serverError(const QString&);
    void signal_serverStarted();
    void newDataAvaible(const QJsonDocument&);
    void signal_newMessage(const QJsonDocument&);

};

#endif // SESSION_H
