#include "session.h"

session::session(QTcpSocket * client)
{
    this->client = client;

    connect (this->client, &QTcpSocket::readyRead, this, &session::newData);
    connect(this, &session::newDataAvaible, this, &session::doCommand);
}

void session::doCommand(const QJsonDocument &doc)
{

        int commandCode = doc.object()["commandCode"].toInt();
        qDebug() << doc.object()["commandCode"].toString();

        switch (commandCode) {
            case commandCode::auth: auth(doc);
            break;
            case commandCode::reg: registration(doc);
            break;
            case commandCode::message: messages(doc);
            break;
        default:
            qDebug() << "Неизвестный код команды";
        }
}

void session::auth(const QJsonDocument &doc){

    qDebug() << "auth!!" << doc.toJson();
    QString login = doc.object()["login"].toString();
    QString password = doc.object()["password"].toString();
    QJsonObject jobj;
    QSqlQuery query;
    QString id;

    query.exec(QString("SELECT id FROM users "
               "WHERE login = '%1' AND password = '%2'").arg(login).arg(password));

    if (query.next()) {
        jobj ["commandCode"] = commandCode::auth;
        query.next();
        int idUser = query.value(0).toInt();
        jobj ["id"] = idUser;
        jobj["rooms"] = roomSend(idUser, query.value(0).toInt());
        qDebug() << "Успешная авторизация!";
        //answer to client commandCode::auth
    }
    else {
        jobj ["commandCode"] = commandCode::invalidAduth;
       //answer commandCode::invalidAduth
    }
    QJsonDocument json(jobj);
    sendData(json);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void session::registration(const QJsonDocument &doc)
{
    qDebug() << "registration\n" << doc.toJson();
    QSqlQuery query;
    QJsonObject jobj;
    QString login = doc.object()["login"].toString();
    QString password = doc.object()["password"].toString();
    QString name = doc.object()["name"].toString();


    query.exec(QString("SELECT id FROM users"
               "WHERE login = %1").arg(login));

    if(query.next()){
        jobj ["commandCode"] = commandCode::loginAlreadyUsed;
        qDebug() << "Логин занят " << query.lastError().text();
        //answer to client commandCode::loginAlreadyUsed
    }
    else{

            if(query.exec(QString("INSERT INTO users(login, password, name) "
                                  "VALUES ('%1', '%2','%3')").arg(login).arg(password).arg(name))){
                qDebug() << "Запись успешно добавлена";
                jobj ["commandCode"] = commandCode::reg;     //answer to client commandCode::reg
                query.exec(QString("SELECT id FROM users "
                                   "WHERE login = '%1'").arg(login));
                query.next();
                int idUser = query.value(0).toInt();
                jobj ["id"] = idUser;
                jobj["rooms"] = roomSend(idUser);

            }
            else{
                jobj ["commandCode"] = commandCode::dataBaseError;    //answer to client commandCode::dataBaseError
                qDebug() << "\nОшибка создания записи: " << query.lastError().text() << "\n";


            }
    }
    QJsonDocument json(jobj);
    sendData(json);
}
/////////////
void session::sendData(const QJsonDocument &data)
{

        qint32 packageSize = data.toJson().length();
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        QString stringData = data.toJson();

        stream << packageSize;
        stream << stringData.toLocal8Bit();

        qDebug() <<"\nSend: \n" << data << "\n" << buffer << "\nVolue package: " << data.toJson().length() << "\nSend Over \n\n";
        qDebug() << client->write(buffer);
        client->waitForBytesWritten(3000);
}

void session::newData()
{

        if (client->bytesAvailable() < 4) {
        return;
        }

    quint32 packageSize;
    QByteArray buffer(client->read(4));
    QDataStream stream(buffer);

    stream >> packageSize;
    packageSize+=4;

    buffer.clear();
    while (buffer.size() < packageSize) {
        if (client->bytesAvailable() <= (packageSize - buffer.length())) {
           buffer.append(client->readAll());
        }
        else {
            buffer.append(client->read(packageSize - buffer.length()));
        }
    }
    buffer.remove(0, 4);
//    QString str;
//    stream >> str;
    //QString str(buffer);

    QJsonDocument json = QJsonDocument::fromJson(buffer);
    qDebug() << "Read:\n" << buffer << "\n" << packageSize << "\n" << json.toJson() << "\nReadOver\n\n";
    emit newDataAvaible(json);
}

void session::messages(const QJsonDocument &doc)
{
    int idRoom = doc.object()["idRoom"].toInt();
    QString name = doc.object()["user"].toString();
    QString message = doc.object()["message"].toString();


}

QJsonObject session::roomSend(int idUser, int idRoom)
{
    QJsonObject  mapRooms;
    QJsonObject  mapUsersForRoom;
    QJsonObject jobj;
    QJsonArray arrRooms;
    QJsonArray arrUsersForRooms;
    QSqlQuery query;


    //init map for rooms
    QString queryText = "SELECT room_users.room_id, room.name "
                       "FROM room_users "
                       "INNER JOIN room ON room.id = room_users.room_id "
                       "WHERE room_users.user_id = '%1' ";
    queryText.arg(idUser);
    qDebug() << "\n\n queryText: " << queryText << "\n\n";
    query.exec(queryText);

//    query.exec(QString("SELECT id, name FROM room "
//                       "WHERE id IN ( SELECT room_id FROM room_users "
//                                    "WHERE user_id = '%1'").arg(idUser));
    qDebug() << "\n\nError : " << query.lastError() << "\n\n";

    while(query.next()){
        mapRooms [query.value(0).toString()] = query.value(1).toString();
    }


//    while(iterMap.hasNext()){
//        iterMap.next();
//        jobj.insert(iterMap.key(), iterMap.value());
//    }

    jobj.insert("rooms", mapRooms);

    //qDebug() << "\n\n Map rooms: " << jobj << "\n\n";
    //iniciat map for users_room
    query.exec(QString("SELECT id, name FROM users "
                       "WHERE id IN (SELECT user_id FROM room_users "
                       "WHERE room_id = '%1')").arg(idRoom));
    while(query.next()){
        mapUsersForRoom [query.value(0).toString()] = query.value(1).toString();
    }

    jobj.insert("usersForRoom", mapUsersForRoom);

//    iterMap = QMapIterator <QString, QString> (mapUsersForRoom);

//    while(iterMap.hasNext()){
//        iterMap.next();
//        jobj.insert(iterMap.key(), iterMap.value());
//    }



 qDebug() << "\n\n Map users: " << jobj << "\n\n";

    return jobj;


}

