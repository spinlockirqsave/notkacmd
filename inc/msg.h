/*
 * msg.h
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


#ifndef MSG_H
#define MSG_H

#include "../inc/websocketsession.h"
#include "../inc/globals.h"

#include <QtWebSockets/QWebSocket>

#include <memory>

class MsgRX;

/**
 * @brief Make the "ds >> msg" notation possible.
 * @param ds    The data stream to be read.
 * @param msg   The message to be initialised from the stream @ds.
 */
QDataStream& operator>>(QDataStream &ds, std::unique_ptr<MsgRX> &msg);

enum MsgErr {
        Ok = 0,
        Fail = 1,
        ErrorTextLen = 2                /* Text length < 0 */
};

struct MsgRX
{
        enum Id {
                IdMsgUnknown = -1,
                IdMsgHandshakeSyn = 1,
                IdMsgLogin = 2,
                IdMsgSaveReq = 3
        };

        virtual void process(QDataStream &ds) = 0;

        explicit MsgRX(Id payload_id, int payload_len, WebSocketSession &ws_session) :
                payload_id(payload_id),
                payload_len(payload_len),
                ws_session(ws_session)
        {}
        virtual ~MsgRX() {}
        MsgRX(const MsgRX&) = delete;
        MsgRX& operator=(const MsgRX&) = delete;

protected:
        QString read_raw_string(QDataStream &ds, int bytes) const;

        Id                      payload_id;
        int                     payload_len;
        WebSocketSession        &ws_session;
};

struct MsgTX
{
        enum Id {
                IdMsgUnknown = -1,
                IdMsgHandshakeAck = 1,
                IdMsgLoginAck = 2,
                IdMsgSaveReqAck = 3,
                IdMsgNotka = 4
        };

        /**
         * @brief Tx header.
         */
        virtual void post();

        explicit MsgTX(Id payload_id, int payload_len, WebSocketSession &ws_session) :
                payload_id(payload_id),
                payload_len(payload_len),
                ws_session(ws_session)
        {}
        virtual ~MsgTX() {}

protected:

        Id                      payload_id;
        int                     payload_len;
        WebSocketSession        &ws_session;
};

class MsgHandshakeSyn : public MsgRX {
public:
        explicit MsgHandshakeSyn(int payload_len, WebSocketSession &ws_session) :
                MsgRX(Id::IdMsgHandshakeSyn, payload_len, ws_session)
        {}
        ~MsgHandshakeSyn() {}
        void process(QDataStream &ds) override;
};

class MsgHandshakeAck : public MsgTX {
public:
        explicit MsgHandshakeAck(int payload_len, WebSocketSession &ws_session) :
                MsgTX(Id::IdMsgHandshakeAck, payload_len, ws_session)
        {}
        ~MsgHandshakeAck() {}
        void post() override;
};

class MsgSaveReq : public MsgRX {
public:
        explicit MsgSaveReq(int payload_len, WebSocketSession &ws_session) :
                MsgRX(Id::IdMsgSaveReq, payload_len, ws_session)
        {}
        ~MsgSaveReq() {}
        MsgSaveReq(const MsgSaveReq&) = delete;
        MsgSaveReq& operator=(const MsgSaveReq&) = delete;
        void process(QDataStream &ds) override;
};

class MsgLogin : public MsgRX {
public:
        explicit MsgLogin(int payload_len, WebSocketSession &ws_session) :
                MsgRX(Id::IdMsgLogin, payload_len, ws_session)
        {}
        ~MsgLogin() {}
        MsgLogin(const MsgLogin&) = delete;
        MsgLogin& operator=(const MsgLogin&) = delete;
        void process(QDataStream &ds) override;
};

class MsgLoginAck : public MsgTX {
public:
        explicit MsgLoginAck(WebSocketSession &ws_session) :
                MsgTX(Id::IdMsgLoginAck, 1, ws_session)
        {}
        ~MsgLoginAck() {}
        void post() override;
};

class MsgSaveReqAck : public MsgTX {
public:
        explicit MsgSaveReqAck(WebSocketSession &ws_session, uint8_t error_code = MsgErr::Ok) :
                MsgTX(Id::IdMsgSaveReqAck, 1, ws_session), error_code(error_code)
        {}
        ~MsgSaveReqAck() {}
        void post() override;
        void post(uint8_t error_code) { this->error_code = error_code; post(); }
private:
        uint8_t error_code;
};

class MsgNotka : public MsgTX {
public:
        explicit MsgNotka(WebSocketSession &ws_session, QByteArray &notka) :
                MsgTX(Id::IdMsgNotka, notka.size(), ws_session), notka(notka)
        {}
        ~MsgNotka() {}
        void post() override;
private:
        QByteArray &notka;
};
#endif // MSG_H
