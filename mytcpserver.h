#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class MyTcpServer : public QTcpServer {
    Q_OBJECT
public:
    MyTcpServer();

private slots:
    void newConnection();
    void readData();
    void disconnect();

private:
    QMap<QTcpSocket*, QString> users;
    void sendTo(QTcpSocket* s, QString msg);
};

#endif // MYTCPSERVER_H