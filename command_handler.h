#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <QTcpSocket>
#include <QMap>

class CommandHandler {
public:
    CommandHandler();

    // Обработка команды
    void handle(QTcpSocket* socket, const QString& data, QMap<QTcpSocket*, QString>& users);

private:
    void sendTo(QTcpSocket* s, const QString& msg);

    void handleHelp(QTcpSocket* s);
    void handleRegister(QTcpSocket* s, const QStringList& parts);
    void handleLogin(QTcpSocket* s, const QStringList& parts, QMap<QTcpSocket*, QString>& users);
    void handleHistory(QTcpSocket* s, const QMap<QTcpSocket*, QString>& users);
    void handleStats(QTcpSocket* s);
    void handleTask(QTcpSocket* s, const QString& data, QMap<QTcpSocket*, QString>& users);
};

#endif // COMMAND_HANDLER_H