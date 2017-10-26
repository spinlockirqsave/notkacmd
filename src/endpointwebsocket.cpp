/*
 * endpointwebsocket.cpp
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


#include "../inc/endpointwebsocket.h"

#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QDebug>


EndPointWebSocket::EndPointWebSocket(QWebSocketServer::SslMode mode,
                                     QHostAddress address,
                                     quint16 port,
                                     QObject *parent) :
        QObject(parent),
        ws_server(new QWebSocketServer(QStringLiteral("Notka WebSocket EndPoint"), mode, this)),
        address(address),
        port(port),
        ws_sessions(),
        thread_pool(parent)
{
        connect(ws_server.data(), SIGNAL(newConnection()), this, SLOT(on_new_connection()));
}

EndPointWebSocket::~EndPointWebSocket()
{
        close();
}

int EndPointWebSocket::open()
{
        if (ws_server && ws_server->listen(address, port)) {
                qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                         << "Listening on address " << address.toString() << " port " << port << "...";
                return 0;
        }

        qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                 << "Err, listen on address " << address.toString() << " port " << port << " failed.";
        return -1;
}

void EndPointWebSocket::close()
{
        ws_sessions_disconnect();

        if (ws_server) {
                ws_server->close();
                qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                         << "Stopped listening on address " << address.toString() << " port " << port << "...";
        }
}

void EndPointWebSocket::ws_sessions_disconnect()
{
        QMap<QWebSocket*, WebSocketSession*>::iterator it = ws_sessions.begin();
        auto end = ws_sessions.end();

        while (it != end)
        {
                it = close_ws_session(it);
        }
}

void EndPointWebSocket::on_new_connection()
{
        /* QWebSocketServer owns the socket pointer. */
        QWebSocket *client = ws_server->nextPendingConnection();

        WebSocketSession *ws_session = new WebSocketSession(*client, *this);

        /**
         * The &QWebSocket::textMessageReceived
         * and &QWebSocket::binaryMessageReceived signals
         * get connected to ws_session's slots. Only &WebSocket::disconnected
         * signal is handled by this end point because it must remove
         * the session from the container and the access to container
         * is synchronised for this class, as EndPointWebSocket's callbacks
         * are called synchronously by main thread only.
         */
        connect(client, &QWebSocket::disconnected, this, &EndPointWebSocket::on_sock_disconnect);

        ws_sessions[client] = ws_session;
}

void EndPointWebSocket::on_sock_disconnect()
{
        auto *client = qobject_cast<QWebSocket *>(sender());

        if (client) {
                auto it = ws_sessions.find(client);

                if (it == ws_sessions.end())
                        return;

                close_ws_session(it);
        }
}

void EndPointWebSocket::bin_msg_tx(QWebSocket *ws, QByteArray raw_msg)
{
                ws->sendBinaryMessage(raw_msg);
                ws->flush();
}

QMap<QWebSocket*, WebSocketSession*>::iterator EndPointWebSocket::close_ws_session(QMap<QWebSocket*, WebSocketSession*>::iterator it)
{
        auto ws_session = it.value();

        while(1) {
                QMutexLocker lock(&ws_session->mutex);
                if (ws_session->qrunnables_scheduled == 0)
                {
                        it = ws_sessions.erase(it);
                        lock.unlock();

                        /* Will deleteLater() web socket in session's destructor. */
                        delete ws_session;
                        return it;
                }
                lock.unlock();
                QThread::msleep(50);
        }
}
