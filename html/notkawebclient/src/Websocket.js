/*
 * Websocket.js
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


var React = require('react');
var ReactDOM = require('react-dom');

var FL = require('./FormLogin.js');
var FN = require('./FormNotka.js');


var wsUri = "ws://notka.online:1235";
//var wsUri = "ws://localhost:1235";
var websocket = null;
var endiannes = 0;      // 0 - le, 1 - be

function swap32(val) {
    return ((val & 0xFF) << 24)
           | ((val & 0xFF00) << 8)
           | ((val >> 8) & 0xFF00)
           | ((val >> 24) & 0xFF);
}

function check_endianness() {
    var arrayBuffer = new ArrayBuffer(2);
    var uint8Array = new Uint8Array(arrayBuffer);
    var uint16array = new Uint16Array(arrayBuffer);
    uint8Array[0] = 0xAA; // set first byte
    uint8Array[1] = 0xBB; // set second byte

    if (uint16array[0] === 0xBBAA)
        endiannes = 0; // le
    else if (uint16array[0] === 0xAABB)
        endiannes = 1; // be
    else {
        endiannes = 2;
        throw new Error("Weird ediannes!");
    }
}

function str2ab(str) {
    var buf = new ArrayBuffer(str.length); // but should be 2 bytes for each char for Unicode
    var bufView = new Uint8Array(buf);
    for (var i=0, strLen=str.length; i<strLen; i++) {
        bufView[i] = str.charCodeAt(i);
    }
    return buf;
}

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint16Array(buf));
}

/**
 * Creates a new Uint8Array based on two different ArrayBuffers
 *
 * @private
 * @param {ArrayBuffers} buffer1 The first buffer.
 * @param {ArrayBuffers} buffer2 The second buffer.
 * @return {ArrayBuffers} The new ArrayBuffer created out of the two.
 */
var _appendBuffer = function(buffer1, buffer2) {
  var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
  tmp.set(new Uint8Array(buffer1), 0);
  tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
  return tmp.buffer;
};

function waitForConnection (callback, interval) {
    if (websocket.readyState === 1) {
        callback();
    } else {
        setTimeout(function () {
            waitForConnection(callback, interval);
        }, interval);
    }
};

function send_wait (msg) {

    waitForConnection(function () {
        websocket.send(msg);
    }, 1000);
}

function tx_MsgSYN() {
        var msg = new ArrayBuffer(8);
        var bufView = new Uint32Array(msg);     // by default js uses big endian
        bufView[0] = 1;
        bufView[1] = 0;
        if ( websocket != null ) {
                if (endiannes === 0) {          // but if this machine is le
                        console.log("swapping");
                        bufView[0] = swap32(bufView[0]);
                        bufView[1] = swap32(bufView[1]);
                }

                send_wait(msg);
                console.log( "sent MsgSYN :", '"'+msg+'"' );
                debug(msg);
        }
}

function Greeting(name) {
  const element = (
    <div>
      <h1>{name}</h1>
    </div>
  );
  ReactDOM.render(element, document.getElementById('main'));
}

var WsState = {
    LOGIN:      0,
    LOGIN_PASS: 1,
    LOGGED_IN:  2,
    LOGIN_FAIL: 3,
    REGISTER: 4,
    REGISTERED: 5
}

var SrvLoginState = {
    INIT: 0,
    LOGGED_IN: 1,
    NO_SUCH_USER: 2,
    WRONG_PASSWORD: 3,
    REGISTERED: 4,
    REGISTER_FAIL: 5
}

var ws_state = WsState.LOGIN;
var login_ = '';
var password_ = '';
var props_;

function login() {
    return login_;
}

function rx_msg_login_ack(data) {
    var index = require('./index.js');
    clearInterval(index.interval_id);
    var raw_msg = new Uint8Array(data);
    var error_code = raw_msg[8];
    var loginState = FL.FormLogin.getLoginState();

    if (error_code === SrvLoginState.LOGGED_IN) {
            // Login successful.
            Greeting(login_);
            ReactDOM.render(React.createElement(FN.FormNotka), document.getElementById('Notka-text'));
    } else if (error_code === SrvLoginState.NO_SUCH_USER) {
            // No such user.
            if (loginState === WsState.REGISTER) {
                    // User clicked Register and sent login/pass so backend responded
                    // no such user message. Need to resend same login/pass to actually register
                    // in the database.
                    tx_msg_login(login_, password_);
                    return;
            }
            FL.FormLogin.updateLoginState(WsState.REGISTER);
            ReactDOM.render(<FL.FormLogin />, document.getElementById('root'));
    } else if (error_code === SrvLoginState.WRONG_PASSWORD) {
            // Wrong password, but not really the first time we hit this (LOGIN state).
            var status = FL.FormLogin.getLoginState();
            if (status === WsState.LOGIN) {
                // Now give password.
                FL.FormLogin.updateLoginState(WsState.LOGIN_PASS);
                ReactDOM.render(<FL.FormLogin login={login_} />, document.getElementById('root'));
            } else {
                    // Wrong password.
                    FL.FormLogin.updateLoginState(WsState.LOGIN_FAIL);
                    ReactDOM.render(<FL.FormLogin />, document.getElementById('root'));
            }
    } else if (error_code === SrvLoginState.REGISTERED) {
            FL.FormLogin.updateLoginState(WsState.REGISTERED);
            ReactDOM.render(<FL.FormLogin />, document.getElementById('root'));
    } else if (error_code === SrvLoginState.REGISTER_FAIL) {
            FL.FormLogin.updateLoginState(WsState.REGISTER_FAIL);
            ReactDOM.render(<FL.FormLogin />, document.getElementById('root'));
    } else {
        alert("Unknown login state response received...")
        FL.FormLogin.updateLoginState(WsState.REGISTER_FAIL);
        ReactDOM.render(<FL.FormLogin />, document.getElementById('main'));
    }
}

function rx_msg_notka(data) {
    var raw_msg = new Uint8Array(data);
    var raw_notka = raw_msg.subarray(8);
    //var dec = new TextDecoder();
    var text = ab2str(raw_notka);
    FN.FormNotka.setNotka(text);
    //ReactDOM.render(<FN.FormNotka />, document.getElementById('container'));
    //ReactDOM.render(render_header, document.getElementById('container').parentNode);
    ReactDOM.render(React.createElement(FN.FormNotka), document.getElementById('Notka-text'));
}

var MsgErr = {
    Ok: 0,
    Fail: 1,
    ErrorTextLen: 2
}

function rx_msg_save_req_ack(data) {
    var raw_msg = new Uint8Array(data);
    var error_code = raw_msg[8];

    var fn = ReactDOM.render(React.createElement(FN.FormNotka), document.getElementById('Notka-text'));
    
    switch (error_code) {

        case MsgErr.Ok: {
            fn.alert_ok();
            break;
        }
        case MsgErr.ErrorTextLen: {
            fn.alert_err();
            break;
        }
        default: {          /* Fail */
            fn.alert_fail();
            break;
        }
    }
}

function debug(message) {
    console.log(message);
}

var MsgTXId = {
        IdMsgUnknown            : -1,
        IdMsgHandshakeAck       : 1,
        IdMsgLoginAck           : 2,
        IdMsgSaveReqAck         : 3,
        IdMsgNotka              : 4
}


var tx_msg_login = function(login, pass) {

        login_ = login;
        password_ = pass;
        var msg = new ArrayBuffer(8);             // 4 bytes - id, 4 - len
        var bufView32 = new Uint32Array(msg);     // by default js uses big endian
        bufView32[0] = 2;                         // MsgRX::MsgLogin (rx from server point of view)
        bufView32[1] = 64;                        // payload length, login 32 bytes + pass 32 bytes
        if (endiannes === 0) {                    // but if this machine is le
                console.log("swapping");
                bufView32[0] = swap32(bufView32[0]);
                bufView32[1] = swap32(bufView32[1]);
        }

        var login_bin = new ArrayBuffer(32);
        var bufView8 = new Uint8Array(login_bin);
        for (var i=0, strLen=login.length; i<strLen && i<32; i++) {
                bufView8[i] = login.charCodeAt(i);
        }

        var pass_bin = new ArrayBuffer(32);
        bufView8 = new Uint8Array(pass_bin);
        for (i=0, strLen=pass.length; i<strLen && i<32; i++) {
                bufView8[i] = pass.charCodeAt(i);
        }

        msg = _appendBuffer(msg, login_bin);
        msg = _appendBuffer(msg, pass_bin);

        if (websocket != null)
        {
                send_wait(msg);
                console.log("tx_msg_login: " + msg);
        }
}

module.exports = {
    initWebSocket: function (props) {

        if (props_ && props_.autologin) {
            return;
        }

        props_ = props;

        try {
            if (websocket && websocket.readyState === 1) {
                return;
            }

            websocket = new WebSocket(wsUri);
            websocket.binaryType = 'arraybuffer';

            websocket.onopen = () => {
                debug("CONNECTED");
                check_endianness();
                tx_MsgSYN();

                if (props_ && props_.autologin) {
                    props_.autologin = false;
                    login_ = props_.login;
                    password_ = props_.pass;
                    tx_msg_login(login_, password_);
                }
            };

            websocket.onclose = function (evt) {
                debug("DISCONNECTED");
            };

            websocket.onmessage = function (evt) {
                debug( evt.data );
                var raw_msg = new Uint32Array(evt.data, 0, 2);
                if (endiannes === 0) {          // but if this machine is le
                        console.log("swapping");
                        raw_msg[0] = swap32(raw_msg[0]);
                        raw_msg[1] = swap32(raw_msg[1]);
                }
                var payload_id = raw_msg[0];
                var payload_len = raw_msg[1];
                debug("Payload id: " + payload_id);
                debug("Payload len: " + payload_len);

                switch (payload_id)
                {
                        case MsgTXId.IdMsgLoginAck: {
                                rx_msg_login_ack(evt.data);
                                break;
                        }
                        case MsgTXId.IdMsgNotka: {
                                rx_msg_notka(evt.data);
                                break;
                        }
                        case MsgTXId.IdMsgSaveReqAck: {
                                rx_msg_save_req_ack(evt.data);
                                break;
                        }

                        default: {
                                break;
                        }
                }
            };

            websocket.onerror = function (evt) {
                debug('ERROR: ' + evt.data);
            };
        } catch (exception) {
            debug('ERROR: ' + exception);
        }
    },
    tx_msg_save_req: function() {
            var msg = new ArrayBuffer(8);             // 4 bytes - id, 4 - len
            var bufView32 = new Uint32Array(msg);     // by default js uses big endian
            bufView32[0] = 3;                         // MsgRX::MsgSaveReq (rx from server point of view)
            bufView32[1] = 0;                         // payload length, 0 so far
            if (endiannes === 0) {                    // but if this machine is le
                    console.log("swapping");
                    bufView32[0] = swap32(bufView32[0]);
                    bufView32[1] = swap32(bufView32[1]);
            }

            var login_bin = new ArrayBuffer(32);
            var bufView8 = new Uint8Array(login_bin);
            for (var i=0, strLen=login_.length; i<strLen && i<32; i++) {
                    bufView8[i] = login_.charCodeAt(i);
            }

            var notka = document.getElementById("NotkaTextArea").value;
            var notka_bin = str2ab(notka);

            bufView32[1] = 32 + notka.length;               // payload length = 32 byte login + notka len
            if (endiannes === 0) {                          // but if this machine is le
                    console.log("swapping");
                    bufView32[1] = swap32(bufView32[1]);
            }

            msg = _appendBuffer(msg, login_bin);
            msg = _appendBuffer(msg, notka_bin);

            if (websocket != null)
            {
                    websocket.send(msg);
                    console.log("tx_msg_save_req: " + msg);
            }
    },
    tx_msg_login: tx_msg_login,
    login_: login_,
    password_: password_,
    props_ : props_,
    ws_state: ws_state,
    WsState: WsState,
    login: login,
};
