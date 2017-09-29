/*
 * FormLogin.js
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

import React, { Component } from 'react';
import './FormLogin.css';


var ws = require('./Websocket.js');

function LoginStatus(text) {
        return <h1>{text}</h1>;
}

export class FormLogin extends Component {

  constructor(props) {
    super(props);
    this.state = {login: '', pass: '', init: 0 };
    if (this.state.init) {
        this.state.init++;
    }
    this.handleChange = this.handleChange.bind(this);
    this.handleSubmit = this.handleSubmit.bind(this);
  }

  static loginState = ws.WsState.LOGIN;

  handleChange(event) {
        var newState = {};
        newState[event.target.name] = event.target.value;
        this.setState(newState);
        //this.state[event.target.name] = event.target.value;
  }

  handleSubmit(event) {
    //alert('Login: ' + this.state.login + ' Pass: ' + this.state.pass);
    event.preventDefault();
    if (FormLogin.loginState === ws.WsState.LOGIN_PASS) {
        var newState = {};
        newState['pass'] = this.refs.btnPass.value;
        this.setState(newState);
        //this.state.pass = this.refs.btnPass.value;
    }
    ws.tx_msg_login(this.state.login, this.state.pass);
  }

  static getLoginState() {
        return FormLogin.loginState;
  }

  static updateLoginState(status) {
        FormLogin.loginState = status;
  }

  render() {
        let label = null;
        if (FormLogin.loginState === ws.WsState.LOGIN ) {
            label = <div>
            <h1><br/>Login</h1>
            <input type="text" name="login" onChange={this.handleChange} />
          </div>;
        } else if (FormLogin.loginState === ws.WsState.LOGIN_PASS) {
                    /*var divStyle = {
                        color: 'green',
                        width: '200',
                        //position: 'relative',
                        top: '100%',
                        left: '0%',
                        margintop: '0em',
                        marginleft: '-0em',
                    };*/
                    label = <div>
                      <h1><br/>Password</h1>
                      <input type="text" name="pass" onChange={this.handleChange} ref="btnPass" />
                    </div>;
        } else if (FormLogin.loginState === ws.WsState.LOGGED_IN) {
                    return LoginStatus("OK");
        } else if (FormLogin.loginState === ws.WsState.LOGGED_FAIL) {
                    return LoginStatus("Logging failed");
        } else if (FormLogin.loginState === ws.WsState.REGISTER ) {
                    label = <div>
                        <h1><br/>Register</h1><br/>
                        Please register.<br/>Leave the password empty if you want to login without password.
                        <h1><br/>Login</h1>
                        <input type="text" name="login" onChange={this.handleChange} />
                        <h1><br/>Password</h1>
                        <input type="text" name="pass" onChange={this.handleChange} />
                        </div>;
        } else if (FormLogin.loginState === ws.WsState.REGISTERED) {
                    return LoginStatus("OK. You are registered as " + this.state.login);
        } else {
                    return LoginStatus("Sorry, but registration failed. Please try again.");
        }

        return (
            <div>
                <form onSubmit={this.handleSubmit}>
                {label}
                <input type="submit" value="Go" />
                </form>
            </div>
        );
  }
}

export default FormLogin;
