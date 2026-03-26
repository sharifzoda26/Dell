#include "mytcpserver.h"
#include "database.h"
#include "method_factory.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {
    Database::getInstance();
    connect(this, &QTcpServer::newConnection, this, &MyTcpServer::newConnection);
    listen(QHostAddress::Any, 33333);
    qDebug() << "Server on 33333";
}

void MyTcpServer::newConnection() {
    QTcpSocket* s = nextPendingConnection();
    users[s] = "";
    connect(s, &QTcpSocket::readyRead, this, &MyTcpServer::readData);
    connect(s, &QTcpSocket::disconnected, this, &MyTcpServer::disconnect);
    sendTo(s, "Welcome! Commands: /reg user pass group, /login user pass, /history, /stats");
}

void MyTcpServer::readData() {
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;

    QString data = QString::fromUtf8(s->readAll()).trimmed();
    qDebug() << "From" << users[s] << ":" << data;

    if (data == "/help") {
        QString help = "\r\n COMMANDS \r\n";
        help += "/reg user pass group - register\r\n";
        help += "/login user pass - login\r\n";
        help += "/history - your requests\r\n";
        help += "/stats - global statistics\r\n";
        help += "/help - this help\r\n";
        help += "Task format: variant a b function\r\n";
        help += "Example: 1 1 2 x^3 - x - 2\r\n";
        help += "\r\n> ";
        sendTo(s, help);
        return;
    }

    if (data.startsWith("/reg")) {
        QStringList p = data.split(' ');
        if (p.size() >= 4) {
            if (Database::getInstance()->registerUser(p[1], p[2], p[3]))
                sendTo(s, "Registered");
            else sendTo(s, "Error");
        }
        return;
    }

    if (data.startsWith("/login")) {
        QStringList p = data.split(' ');
        if (p.size() >= 3) {
            if (Database::getInstance()->loginUser(p[1], p[2])) {
                users[s] = p[1];
                sendTo(s, "OK");
            }
            else sendTo(s, "Fail");
        }
        return;
    }

    if (data == "/history") {
        if (users[s].isEmpty()) sendTo(s, "Login first");
        else sendTo(s, Database::getInstance()->getHistory(users[s]));
        return;
    }

    if (data == "/stats") {
        sendTo(s, Database::getInstance()->getStats());
        return;
    }

    if (users[s].isEmpty()) {
        sendTo(s, "Login first");
        return;
    }
    

    QStringList p = data.split(' ', Qt::SkipEmptyParts);
    if (p.isEmpty()) return;

    bool ok;
    int v = p[0].toInt(&ok);
    if (!ok) { sendTo(s, "First must be number"); return; }

    QString inp;
    for (int i = 1; i < p.size(); i++) inp += p[i] + " ";

    Method* m = MethodFactory::get(v);
    QString res = m->execute(inp);
    delete m;

    Database::getInstance()->saveRequest(users[s], v, inp, res);
    sendTo(s, res);
}

void MyTcpServer::sendTo(QTcpSocket* s, QString msg) {
    s->write((msg + "\r\n> ").toUtf8());
}

void MyTcpServer::disconnect() {
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (s) {
        users.remove(s);
        s->deleteLater();
    }
}