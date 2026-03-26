#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include <QDebug>

class Database : public QObject {
    Q_OBJECT

private:
    static Database* instance;
    QSqlDatabase db;

    Database();

public:
    static Database* getInstance();
    bool registerUser(QString username, QString password, QString group);
    bool loginUser(QString username, QString password);
    bool saveRequest(QString username, int variant, QString input, QString output);
    QString getHistory(QString username);
    QString getStats();
};

#endif