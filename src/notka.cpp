/*
 * notka.cpp
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


#include <QCloseEvent>
#include <QMessageBox>

#include <list>

#include "../inc/notka.h"


Notka::Notka(QWidget *parent) :
        QObject(parent),
        endpoints(),
        task_list()
{

        /* Add default server. */
        auto endpoint = add_ws_server();

        if (endpoint)
                ws_server_start_stop(*endpoint, 1);

        /* Signals. */
        connect(this, SIGNAL(db_reconnect_signal()), this, SLOT(db_reconnect()));

        if (!Db::init_database())
                throw std::runtime_error("Database cannot be opened");

        /* Start tasks. */
        start_db_reconnect_task();
}

Notka::~Notka()
{
        for (auto &task : task_list) {
                task->stop();
        }

        for (auto &endpoint : endpoints) {
                endpoint->close();
        }

        Db::close_database();
}

void Notka::db_reconnect()
{
        Db::reconnect();
}

void Notka::start_db_reconnect_task()
{
        /* Reconnect to database every 1 hour.
         * 8 hours is default wait_timeout in MySql. */
        task_list.push_back(std::make_unique<Db::DbReconnectTask>(1000*60*60*1, *this));
}

QSharedPointer<NotkaEndPoint> Notka::ws_server_add(QWebSocketServer::SslMode mode,
                          QHostAddress address, quint16 port)
{
        /* Add new Web Socket server. */
        QSharedPointer<NotkaEndPoint> e(new EndPointWebSocket(mode, address, port));
        endpoints.append(e);
        return e;
}

void Notka::ws_server_start_stop(NotkaEndPoint &endpoint, bool start)
{

        if (start)
                endpoint.open();
        else
                endpoint.close();
}

QSharedPointer<NotkaEndPoint> Notka::add_ws_server()
{
        QHostAddress ip(QHostAddress::Any);
        quint16 port = 1235;

        /* Insert socket into container. */
        return ws_server_add(QWebSocketServer::NonSecureMode, ip, port);
}

/* Signals. */
