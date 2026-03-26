#include "database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

Database* Database::instance = nullptr;

Database::Database() : QObject(nullptr) {
    qDebug() << " БД создана (один раз) ";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("server.db");

    if (!db.open()) {
        qDebug() << "DB error:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE,"
        "password TEXT,"
        "group_name TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT,"
        "variant INTEGER,"
        "input TEXT,"
        "output TEXT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");

    qDebug() << "Database ready";
}

Database* Database::getInstance() {
    if (!instance) instance = new Database();
    return instance;
}

bool Database::registerUser(QString username, QString password, QString group) {
    QSqlQuery q;
    q.prepare("INSERT INTO users (username, password, group_name) VALUES (?,?,?)");
    q.addBindValue(username);
    q.addBindValue(password);
    q.addBindValue(group);
    return q.exec();
}

bool Database::loginUser(QString username, QString password) {
    QSqlQuery q;
    q.prepare("SELECT * FROM users WHERE username=? AND password=?");
    q.addBindValue(username);
    q.addBindValue(password);
    q.exec();
    return q.next();
}

bool Database::saveRequest(QString username, int variant, QString input, QString output) {
    QSqlQuery q;
    q.prepare("INSERT INTO history (username, variant, input, output) VALUES (?,?,?,?)");
    q.addBindValue(username);
    q.addBindValue(variant);
    q.addBindValue(input);
    q.addBindValue(output);
    return q.exec();
}

QString Database::getHistory(QString username) {
    QSqlQuery q;
    q.prepare("SELECT variant, input, output, timestamp FROM history WHERE username=? ORDER BY timestamp DESC LIMIT 10");
    q.addBindValue(username);
    q.exec();

    QString res = "\n=== HISTORY ===\n";
    while (q.next()) {
        res += QString("%1 | var %2 | %3 -> %4\n")
            .arg(q.value(3).toString())
            .arg(q.value(0).toInt())
            .arg(q.value(1).toString())
            .arg(q.value(2).toString());
    }
    return res;
}

QString Database::getStats() {
    QSqlQuery q;
    q.exec("SELECT variant, COUNT(*) FROM history GROUP BY variant");

    QString res = "\n=== STATS ===\n";
    while (q.next()) {
        res += QString("Variant %1: %2 calls\n").arg(q.value(0).toInt()).arg(q.value(1).toInt());
    }
    return res;
}