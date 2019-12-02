#include "session.h"

session::session(QTcpSocket * client)
{
    this->client = client;

    connect (this->client, &QTcpSocket::readyRead, this, &session::newData);
    connect(this, &session::newDataAvaible, this, &session::doCommand);
}

void session::doCommand(const QJsonDocument &doc) //modify
{
        qDebug() << "Command string pack " << doc.toJson() << "\n";
        int commandCode = doc.object()["commandCode"].toInt();
        //qDebug() << doc.object()["commandCode"].toString();

        switch (commandCode) {
            case commandCode::auth: auth(doc);
            break;
            case commandCode::reg: registration(doc);
            break;
            case commandCode::message: newMessag(doc);
            break;
             case commandCode::roomCreated: creatRoom(doc);
            break;
            case commandCode::sendRoomId : sendRoomOnClick(doc);
            break;
            //case commandCode::invite : ;
           // break;
        default:
            qDebug() << "Неизвестный код команды";
        }
}

void session::auth(const QJsonDocument &doc){ //modify

    qDebug() << "auth!!" << doc.toJson();
    QString login = doc.object()["login"].toString();
    QString password = doc.object()["password"].toString();
    QJsonObject jobj;
    QSqlQuery query;

    query.exec(QString("SELECT id FROM users "
               "WHERE login = '%1' AND password = '%2'").arg(login).arg(password));

    if (query.next()) {
        jobj ["commandCode"] = commandCode::auth;
        //query.next();
        idThisUser = query.value(0).toInt();
        jobj ["id"] = idThisUser;
        jobj["rooms"] = sendRoom();
        jobj["users"] = SendRoomUsers();


// send to room

        qDebug() << "Успешная авторизация!";
        //answer to client commandCode::auth
    }
    else {
        jobj ["commandCode"] = commandCode::invalidAuth;
       //answer commandCode::invalidAduth
    }
   // sendHistoryMessage();
    QJsonDocument json(jobj);
    sendData(json);
}

void session::registration(const QJsonDocument &doc) // добавление юзера в СУБД + возврат комнат и его ID
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

                idThisUser = query.value(0).toInt();
                jobj ["id"] = idThisUser;
                jobj["rooms"] = sendRoom();
                jobj["users"] = SendRoomUsers();
            }
            else{
                jobj ["commandCode"] = commandCode::dataBaseError;    //answer to client commandCode::dataBaseError
                qDebug() << "\nОшибка создания записи: " << query.lastError().text() << "\n";
            }
    }
    //sendHistoryMessage();
    QJsonDocument json(jobj);
    sendData(json);
}

void session::sendData(const QJsonDocument &data) // Отправка данных
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

void session::newData() // приём данных
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

QJsonObject session::sendRoom() // список комнат, в которых состоит пользователь
{
    QJsonObject  mapRooms;

    QSqlQuery query;
    QString role;

    //init map for rooms
    query.exec(QString("SELECT room_users.room_id, room.name, room.user_id "
                       "FROM room_users "
                       "INNER JOIN room ON room.id = room_users.room_id "
                       "WHERE room_users.user_id = '%1' ").arg(idThisUser));

    qDebug() << "\n\nError : " << query.lastError() << "\n" << query.lastQuery() << "\n\n";
    while(query.next()){
        QJsonObject mapInRoom;
        if(query.value(2).toInt() != idThisUser){
            role = "0";//no admin
        }
        else{
            role = "1"; // admin
        }
        qDebug() << "Exec value " << query.value(1).toString();

        mapInRoom["isAdmin"] = role;
        mapInRoom["name"] = query.value(1).toString();
        mapRooms [query.value(0).toString()] = mapInRoom;
        qDebug()<< "Map In Room: " << mapInRoom << "\n";
    }
     qDebug() << "MAP for rooms" << mapRooms << "\n";

    return mapRooms;
}



QJsonObject session::SendRoomUsers(int idRoom) //список юзеров комнаты
{
    QJsonObject  mapUsersForRoom;
    QSqlQuery query;

    query.exec(QString("SELECT id, name FROM users "
                       "WHERE id IN (SELECT user_id FROM room_users "
                       "WHERE room_id = '%1')").arg(idRoom));
    while(query.next()){
        mapUsersForRoom [query.value(0).toString()] = query.value(1).toString();
    }

 qDebug() << "\n\n Map users: " << mapUsersForRoom << "\n\n";

 return mapUsersForRoom;
}

void session::creatRoom(const QJsonDocument &doc) //создание комнаты commandCode = 3 // отправить id room
{

    QString nameRoom = doc.object()["roomName"].toString();
    QSqlQuery query;
    QJsonObject jobject;

    if(query.exec(QString("INSERT INTO room (name, user_id) "
                      "VALUES ('%1', '%2' )").arg(nameRoom).arg(idThisUser))){
        jobject ["commandCode"] = commandCode::roomCreated;
        jobject ["rooms"] = sendRoom();
        jobject ["users"] = SendRoomUsers();
    }
    else {
        jobject ["commandCode"] = commandCode::dataBaseError;
        qDebug() << "Error create room" << "\n";
    }

    QJsonDocument jdoc(jobject);
    sendData(jdoc);
}

void session::sendRoomOnClick(const QJsonDocument &doc) //при клике на комнату comandCode = 14
{
    QJsonObject jobj;

    idThisRoom = doc.object()["roomId"].toInt();
    jobj ["commandCode"] = commandCode::sendRoomId;
    jobj["users"] = SendRoomUsers(idThisRoom);

    QJsonDocument jdoc (jobj);
    sendData(jdoc);
}

void session::newMessag(const QJsonDocument &doc) // отправка сообщений // in work
{
    QSqlQuery query;
    QJsonObject jobj;

    QString text = doc.object()["message"].toString();

    if(query.exec(QString("INSERT INTO messages (user_id, room_id, text)"
                       "VALUES ('%1', '%2', '%3') ").arg(idThisUser).arg(idThisRoom).arg(text))){
        qDebug() << "Creat Message is luck!\n";
    }
    else{
        qDebug() << "Creat Message error" << query.lastError() << "\n";
    }
    jobj["commandCode"] = doc.object()["commandCode"];
    jobj["idUser"] = idThisUser;
    jobj["message"] = text;
    jobj["idRoom"] = idThisRoom;
    QJsonDocument docOut(jobj);


    emit signal_newMessage(docOut);
}

void session::sendMessage(const QJsonDocument &doc)
{

    QSqlQuery query;
    if(query.exec(QString("SELECT  "))){

    }

    sendData(doc);
}

void session::sendHistoryMessage()
{

}

void session::sendInviteToRoom(const QJsonDocument &doc)
{
    QSqlQuery query;
    QString idUser = doc.object()["userId"].toString();
    QString idRoom = doc.object()["roomId"].toString();
    if(query.exec(QString("ISERT INTO room_users (room_id, user_id)"
                          "VALUES ('%1', '%2') ").arg(idUser).arg(idRoom))){
    }
}
