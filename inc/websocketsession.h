/*
 * websocketsession.h
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


#ifndef WEBSOCKETSESSION_H
#define WEBSOCKETSESSION_H

#include <QObject>
#include <QString>
#include <QtWebSockets/QWebSocket>
#include <QByteArray>
#include <QMutex>
#include <QRunnable>
#include <QDataStream>
#include <QIODevice>
#include <QScopedPointer>
#include <QThreadPool>

class EndPointWebSocket;
class MsgHandler;


enum WebSocketSessionState {
        INIT = 0,
        LOGGED_IN = 1,
        NO_SUCH_USER = 2,
        WRONG_PASSWORD = 3,
        REGISTERED = 4,
        REGISTER_FAIL = 5
};

class WebSocketSession : public QObject
{
        Q_OBJECT
public:
        explicit WebSocketSession(QWebSocket &ws,
                         EndPointWebSocket const &endpoint);

        /**
         * @brief Close socket and schedule it for defferred deletion.
         */
        ~WebSocketSession();

        WebSocketSession(WebSocketSession const &) = delete;
        WebSocketSession& operator=(WebSocketSession const &) = delete;

        /**
         * @brief Forwards the call to signal bin_msg_tx.
         */
        void bin_msg_tx(QByteArray raw_msg);

public slots:
        void on_text_msg_rx(QString msg);
        void on_bin_msg_rx(QByteArray raw_msg);

        /**
         * Only for Secure Sockets.
         */
        void onSslErrors(const QList<QSslError> &errors);

signals:
        void signal_bin_msg_tx(QWebSocket *ws, QByteArray raw_msg);

public:
        /**
         * @brief Mutex serialising access to this session.
         * Accessed by MsgHandler in QRunnable's run().
         */
        QMutex          mutex;
        int             qrunnables_scheduled;

/**
 * TODO this should be private and session made QRunnable...
 * instead of MsgHandler being QRunnable and processing session
 * which requires session's user data to be public, or MsgHandler
 * being session's friend, what's worse...
 */
        QWebSocket &ws;

        EndPointWebSocket const& endpoint;

        QString                 user;
        WebSocketSessionState   state;

private:

        /*
         * Connections are handled by worker threads from thread pool.
         */
        static QThreadPool      thread_pool;
};

#endif // WEBSOCKETSESSION_H
