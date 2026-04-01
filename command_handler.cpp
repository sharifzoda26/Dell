#include "command_handler.h"
#include "database.h"
#include "method_factory.h"
#include <QDebug>
#include <QStringList>

CommandHandler::CommandHandler() {}

void CommandHandler::sendTo(QTcpSocket* s, const QString& msg) {
    s->write((msg + "\r\n> ").toUtf8());
}

void CommandHandler::handleHelp(QTcpSocket* s) {
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
}

void CommandHandler::handleRegister(QTcpSocket* s, const QStringList& parts) {
    if (parts.size() >= 4) {
        if (Database::getInstance()->registerUser(parts[1], parts[2], parts[3]))
            sendTo(s, "Registered");
        else
            sendTo(s, "Error");
    }
    else {
        sendTo(s, "Error: /reg username password group");
    }
}

void CommandHandler::handleLogin(QTcpSocket* s, const QStringList& parts,
    QMap<QTcpSocket*, QString>& users) {
    if (parts.size() >= 3) {
        if (Database::getInstance()->loginUser(parts[1], parts[2])) {
            users[s] = parts[1];
            sendTo(s, "OK");
        }
        else {
            sendTo(s, "Fail");
        }
    }
    else {
        sendTo(s, "Error: /login username password");
    }
}

void CommandHandler::handleHistory(QTcpSocket* s, const QMap<QTcpSocket*, QString>& users) {
    if (users[s].isEmpty()) {
        sendTo(s, "Login first");
    }
    else {
        sendTo(s, Database::getInstance()->getHistory(users[s]));
    }
}

void CommandHandler::handleStats(QTcpSocket* s) {
    sendTo(s, Database::getInstance()->getStats());
}

void CommandHandler::handleTask(QTcpSocket* s, const QString& data,
    QMap<QTcpSocket*, QString>& users) {
    // Проверка авторизации
    if (users[s].isEmpty()) {
        sendTo(s, "Login first");
        return;
    }

    QStringList parts = data.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    bool ok;
    int variant = parts[0].toInt(&ok);
    if (!ok) {
        sendTo(s, "First must be number");
        return;
    }

    // Формируем входные данные
    QString input;
    for (int i = 1; i < parts.size(); i++) {
        input += parts[i];
        if (i < parts.size() - 1) input += " ";
    }

    // Выполняем метод
    Method* method = MethodFactory::get(variant);
    QString result = method->execute(input);
    delete method;

    // Сохраняем в БД
    Database::getInstance()->saveRequest(users[s], variant, input, result);

    // Отправляем результат
    sendTo(s, result);
}

void CommandHandler::handle(QTcpSocket* socket, const QString& data,
    QMap<QTcpSocket*, QString>& users) {

    if (data == "/help") {
        handleHelp(socket);
        return;
    }

    if (data.startsWith("/reg")) {
        QStringList parts = data.split(' ');
        handleRegister(socket, parts);
        return;
    }

    if (data.startsWith("/login")) {
        QStringList parts = data.split(' ');
        handleLogin(socket, parts, users);
        return;
    }

    if (data == "/history") {
        handleHistory(socket, users);
        return;
    }

    if (data == "/stats") {
        handleStats(socket);
        return;
    }

    
    handleTask(socket, data, users);
}