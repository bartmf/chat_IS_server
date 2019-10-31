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
#include <QList>
#include <QJsonArray>

namespace commandCode {
    const int auth = 10;
    const int reg = 1;
    const int roomUser = 3;
    const int roomName = 4;
    const int invite = 5;
    const int invalidAduth = 11;
    const int loginAlreadyUsed = 12;
    const int message = 2;
    const int dataBaseError = 13;
}

class session:public QObject
{
    Q_OBJECT

    int id;
public:
    QTcpSocket * client;

public:
    session(QTcpSocket * client);
    void doCommand(const QJsonDocument &doc);
    void auth(const QJsonDocument &doc);
    void registration(const QJsonDocument &doc);
    void sendData(const QJsonDocument &data);
    void newData();
    void messages(const QJsonDocument &doc);
    void roomSend(int id);

signals:
    void signal_serverError(const QString&);
    void signal_serverStarted();
    void newDataAvaible(const QJsonDocument&);

};

#endif // SESSION_H
