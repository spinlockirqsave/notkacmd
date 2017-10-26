/*
 * msg.cpp
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


#include "../inc/msg.h"

#include <QDebug>


QDataStream& operator>>(QDataStream &ds, std::unique_ptr<MsgRX> &msg)
{
        msg->process(ds);
        return ds;
}

QString MsgRX::read_raw_string(QDataStream &ds, int bytes) const
{
        char buf[bytes];
        ds.readRawData(buf, sizeof(buf));

        return QString(buf);
}
void MsgTX::post()
{
        QByteArray raw_msg;
        QDataStream ds(&raw_msg, QIODevice::WriteOnly);

        ds << payload_id;
        ds << payload_len;

        ws_session.bin_msg_tx(raw_msg);
}

void MsgHandshakeSyn::process(QDataStream &ds)
{
        /* As for now, just send ACK after "processing" this SYN. */
        (void) ds;
        MsgHandshakeAck ack(0, ws_session);
        ack.post();
}

void MsgHandshakeAck::post()
{
        /* Just send the header */
        MsgTX::post();
        /* No body for ACK. */
        return;
}

void MsgSaveReq::process(QDataStream &ds)
{
        MsgSaveReqAck ack(ws_session);
        int text_len = payload_len - 32;        /* not including login field */

        if (text_len < 0) {
                ack.post(MsgErr::ErrorTextLen);
                return;
        }

        QString login = read_raw_string(ds, 32);

        char* bytes = new char[text_len];
        ds.readRawData(bytes, text_len);
        QByteArray notka = QByteArray::fromRawData(bytes, text_len);

        bool ok = Db::save_notka(login, notka);

        if (!ok) {
                ack.post(MsgErr::Fail);
                return;
        }

        ack.post(MsgErr::Ok);
}

void MsgLogin::process(QDataStream &ds)
{
        MsgLoginAck ack(ws_session);

        if (payload_len != 64) {
                /* Error, where is login and pass?
                 * TODO: Send error description. */
                ack.post();
                return;
        }

        QString login = read_raw_string(ds, 32);
        QString password = read_raw_string(ds, 32);

        ws_session.user = login;

        if (ws_session.state == NO_SUCH_USER) {
                /* Registration. */
                try {
                        Db::register_user(login, password);
                        ws_session.state = REGISTERED;
                } catch (std::exception &e) {
                        ws_session.state = REGISTER_FAIL;
                        qDebug() << __func__ <<
                                    QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                                    << "User " << login << " tried to register, but database "
                                    "transaction failed! [" << e.what() << "]";
                }
        } else {
                int res = Db::authenticate_user(login, password);
                if (res == 0) {
                        ws_session.state = LOGGED_IN;
                } else if (res == 1) {
                        /* No such user. */
                        ws_session.state = NO_SUCH_USER;
                } else {
                        /* Wrong Password. */
                        ws_session.state = WRONG_PASSWORD;
                }
        }

        ack.post();

        if (ws_session.state == LOGGED_IN) {
                /* Check if user has any notes and tx them if they have. */
                QByteArray notka;
                if (Db::get_notka(login, notka)) {
                        MsgNotka user_stuff(ws_session, notka);
                        user_stuff.post();
                }
        }
}

void MsgLoginAck::post()
{
        /* Append the header */
        QByteArray raw_msg;
        QDataStream ds(&raw_msg, QIODevice::WriteOnly);

        ds << payload_id;
        ds << payload_len;

        /* And the error code as body for ACK. */
        uint8_t error_code = ws_session.state;

        ds << error_code;

        /* TX */
        ws_session.bin_msg_tx(raw_msg);
}

void MsgSaveReqAck::post()
{
        QByteArray raw_msg;
        QDataStream ds(&raw_msg, QIODevice::WriteOnly);

        ds << payload_id;
        ds << payload_len;
        ds << error_code;

        ws_session.bin_msg_tx(raw_msg);
}

void MsgNotka::post()
{
        /* Append the header */
        QByteArray raw_msg;
        QDataStream ds(&raw_msg, QIODevice::WriteOnly);

        ds << payload_id;
        /* No need to write the payload_len as the ds << QByteArray
         * serialises it's size. */
        /* ds << payload_len; */

        /* And the user's notes size (4 bytes) and text as the body. */
        ds << notka;

        /* TX */
        ws_session.bin_msg_tx(raw_msg);
}
