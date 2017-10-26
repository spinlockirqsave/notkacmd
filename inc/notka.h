/*
 * notka.h
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


#ifndef NOTKA_H
#define NOTKA_H


#include <QObject>
#include "../inc/globals.h"

#include "../inc/endpointwebsocket.h"


namespace Ui {
class Notka;
}

#define APP_NAME "Notka"

class Notka : public QObject
{
        Q_OBJECT

public:
        explicit Notka(QWidget *parent = 0);
        Notka(Notka const &) = delete;
        Notka& operator=(const Notka &) = delete;
        ~Notka();

public slots:
        void db_reconnect();

private:
        QList<QSharedPointer<NotkaEndPoint>> endpoints;

        std::list<std::unique_ptr<Task>> task_list;
        void start_db_reconnect_task();

        inline QSharedPointer<NotkaEndPoint> ws_server_add(QWebSocketServer::SslMode mode,
                           QHostAddress address, quint16 port);

        QSharedPointer<NotkaEndPoint> add_ws_server();

private slots:
        void ws_server_start_stop(NotkaEndPoint &endpoint, bool start);

signals:
        void db_reconnect_signal();
};

#endif // NOTKA_H
