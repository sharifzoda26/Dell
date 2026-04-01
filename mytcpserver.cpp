#include "mytcpserver.h"
#include "database.h"
#include "command_handler.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {
    Database::getInstance();
    connect(this, &QTcpServer::newConnection, this, &MyTcpServer::newConnection);
    listen(QHostAddress::Any, 33333);
    qDebug() << "========================================";
    qDebug() << "СЕРВЕР ЗАПУЩЕН";
    qDebug() << "Порт: 33333";
    qDebug() << "Ожидание подключений...";
    qDebug() << "========================================";
}

void MyTcpServer::newConnection() {
    QTcpSocket* s = nextPendingConnection();

    // Информация о новом клиенте
    QString clientIP = s->peerAddress().toString();
    quint16 clientPort = s->peerPort();

    // Лог сервера
    qDebug() << "";
    qDebug() << "Новый клиент подключилься !";
    qDebug() << "IP:" << clientIP;
    qDebug() << "Порт:" << clientPort;
    qDebug() << "Всего клиентов:" << (users.size() + 1);
    qDebug() << "";

    users[s] = "";
    connect(s, &QTcpSocket::readyRead, this, &MyTcpServer::readData);
    connect(s, &QTcpSocket::disconnected, this, &MyTcpServer::disconnect);

    
    QString welcome = "\r\n\r\n";
    welcome += "ДОБРО ПОЖАЛОВАТЬ НА СЕРВЕР!\r\n";
    welcome += "Ваш IP: " + clientIP + "\r\n";
    welcome += "Ваш порт: " + QString::number(clientPort) + "\r\n";
    welcome += "\r\n";
    welcome += "Commands: /reg user pass group, /login user pass, /history, /stats\r\n";
    welcome += "Task format: variant a b function\r\n";
    welcome += "Example: 1 1 2 x^3 - x - 2\r\n";
    welcome += "> ";

    sendTo(s, welcome);
}

void MyTcpServer::readData() {
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;

    QString data = QString::fromUtf8(s->readAll()).trimmed();

    // Лог сервера
    QString clientIP = s->peerAddress().toString();
    quint16 clientPort = s->peerPort();
    qDebug() << "[" << clientIP << ":" << clientPort << "]"
        << "Пользователь:" << (users[s].isEmpty() ? "неавторизован" : users[s])
        << "→ Команда:" << data;

    // Обработка команды
    CommandHandler handler;
    handler.handle(s, data, users);
}

void MyTcpServer::sendTo(QTcpSocket* s, QString msg) {
    s->write((msg + "\r\n> ").toUtf8());
}

void MyTcpServer::disconnect() {
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (s) {
        QString clientIP = s->peerAddress().toString();
        quint16 clientPort = s->peerPort();
        QString username = users[s];

        // Лог сервера
        qDebug() << "";
        qDebug() << "КЛИЕНТ ОТКЛЮЧИЛСЯ";
        qDebug() << "IP:" << clientIP;
        qDebug() << "Порт:" << clientPort;
        qDebug() << "Пользователь:" << (username.isEmpty() ? "неавторизован" : username);
        qDebug() << "Осталось клиентов:" << (users.size() - 1);
        qDebug() << "";

        users.remove(s);
        s->deleteLater();
    }
}