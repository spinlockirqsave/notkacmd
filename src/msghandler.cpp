/*
 * msghandler.cpp
 *
 * Copyright(C) 2017, Piotr Gregor <piotrgregor@rsyncme.org>
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


#include <QDebug>

#include "../inc/msghandler.h"


MsgHandler::MsgHandler(QByteArray raw_msg,
                       WebSocketSession &ws_session) :
        raw_msg(raw_msg),
        ws_session(ws_session),
        ds(new QDataStream(&this->raw_msg, QIODevice::ReadOnly))
{
        ds->setByteOrder(QDataStream::BigEndian);
}

std::unique_ptr<MsgRX> MsgHandler::demux_raw_msg(QByteArray raw_msg)
{
        int msg_len = raw_msg.length();
        if (msg_len < 8)
                return nullptr;

        int payload_id;
        *ds >> payload_id;

        int payload_len;
        *ds >> payload_len;

        switch (payload_id)
        {
        case MsgRX::Id::IdMsgHandshakeSyn:
                return std::unique_ptr<MsgRX>(new MsgHandshakeSyn(payload_len, ws_session));

        case MsgRX::IdMsgLogin:
                return std::unique_ptr<MsgRX>(new MsgLogin(payload_len, ws_session));

        case MsgRX::IdMsgSaveReq:
                return std::unique_ptr<MsgRX>(new MsgSaveReq(payload_len, ws_session));

        default:
                return nullptr;
        }
}

void MsgHandler::run()
{
        QMutexLocker ml(&ws_session.mutex);

        qDebug() << __func__ <<
                    QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss") << raw_msg;

        std::unique_ptr<MsgRX> msg = demux_raw_msg(raw_msg);

        if (!msg) {
                /* TODO Tx error description. */
        } else {
                /* Process the message. */
                try {
                        *ds >> msg;
                } catch (std::exception &e) {
                        qDebug() << "Processing msg failed: " << e.what();
                }
        }

        /**
         * Processing finished, so tell end point logic
         * - session can be removed from endpoint if there
         * is no QRunnables scheduled to process that session.
         */
        ws_session.qrunnables_scheduled--;
}
