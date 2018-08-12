/*
 * globals.cpp
 *
 * Copyright(C) 2017, Piotr Gregor <piotr@dataandsignal.com>
 *
 * Notka Online Clipboard
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include "../inc/notka.h"
#include "../inc/globals.h"

#include <QDateTime>
#include <QDebug>


QString db_con_name = "Notka";

bool Db::init_database()
{
        QMutexLocker lock(&Database::mutex);

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", db_con_name);

        db.setHostName("localhost");
        db.setDatabaseName("notka");
        db.setUserName("notka");
        db.setPassword("password");

        return db.open();
}

void Db::close_database()
{
        QMutexLocker lock(&Database::mutex);
        {
                QSqlDatabase db = QSqlDatabase::database(db_con_name);
                if (db.isOpen()) {
                        db.close();
                }
                db = QSqlDatabase();
        }
        QSqlDatabase::removeDatabase(db_con_name);
}

int Db::authenticate_user(QString login, QString password)
{
        qDebug() << "Auth: [" << login << "] [" << password << "]";

        if (login.isEmpty() || login.isNull())
                return 1;

        QMutexLocker lock(&Db::mutex);

        QSqlDatabase db = QSqlDatabase::database(db_con_name);

        if (!db.isOpen() || db.isOpenError())
                throw std::runtime_error("Database is not opened");

        db.transaction();

        QSqlQuery query(db);
        query.exec("SELECT password FROM users WHERE user = '" + login + "'");

        if (query.lastError().isValid()) {
                db.rollback();
                throw std::runtime_error("Database query error "
                                         + query.lastError().text().toStdString());
        }

        db.commit();

        if (!query.size()) {
                /* No such user. */
                return 1;
        }

        while (query.next())
        {
                QString pass = query.value(0).toString();
                if (pass == password)
                        return 0;
        }

        /* Wrong password. */
        return 2;
}

int Db::register_user(QString login, QString password)
{
        if (login.isEmpty() || login.isNull())
                throw std::runtime_error("Login is empty or null");

        QMutexLocker lock(&Db::mutex);
        QSqlDatabase db = QSqlDatabase::database(db_con_name);
        if (!db.isOpen() || db.isOpenError())
                throw std::runtime_error("Databse is not opened");

        db.transaction();
        QSqlQuery query(db);
        query.prepare("INSERT INTO users VALUES (:user, :password)");
        query.bindValue(0, login);
        query.bindValue(1, password);

        query.exec();

        if (query.lastError().isValid()) {
                db.rollback();
                throw std::runtime_error("Database query error "
                                         + query.lastError().text().toStdString());
        }

        db.commit();
        return 0;

}

bool Db::save_notka(QString user, QByteArray notka)
{
        QMutexLocker lock(&Db::mutex);

        QSqlDatabase db = QSqlDatabase::database(db_con_name);

        db.transaction();

        QSqlQuery query(db);
        query.prepare("INSERT INTO notkas (user, notka) VALUES (:user, :notka) "
                      "ON DUPLICATE KEY UPDATE notka = VALUES(notka)");
        query.bindValue(0, user);
        query.bindValue(1, notka);
        query.exec();

        if (query.lastError().isValid()) {
                qDebug() << query.lastError().text();
                db.rollback();
                return false;
        }

        db.commit();

        return true;
}

bool Db::get_notka(QString user, QByteArray &notka)
{
        QMutexLocker lock(&Db::mutex);

        QSqlDatabase db = QSqlDatabase::database(db_con_name);

        db.transaction();

        QSqlQuery query(db);
        query.prepare("SELECT CONVERT(notka USING utf8) FROM notkas WHERE user = :user");
        query.bindValue(0, user);
        query.exec();

        if (query.lastError().isValid()) {
                qDebug() << query.lastError().text();
                db.rollback();
                return false;
        }

        db.commit();

        if (!query.size()) {
                /* No such user */
                return false;
        }

        if (query.next())
                notka = query.value(0).toByteArray();

        return true;
}

void Db::reconnect()
{
        qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                 << "Database reopening task...";

        close_database();
        init_database();

        {
                QSqlDatabase db = QSqlDatabase::database(db_con_name);

                if (db.lastError().isValid()) {
                        throw std::runtime_error("Database reconnect error "
                                         + db.lastError().text().toStdString());
                }
        }
}

Db::DbReconnectTask::DbReconnectTask(int interval_ms, Notka &n)
        : PeriodicTask(interval_ms), notka(n)
{}

Db::DbReconnectTask::~DbReconnectTask()
{
        qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                 << "Goodbye...";
}

bool Db::DbReconnectTask::run_once()
{
        emit notka.db_reconnect_signal();

        return false;
}
