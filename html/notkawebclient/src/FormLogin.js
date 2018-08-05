/*
 * FormLogin.js
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

import React, { Component } from 'react';
import ReactDOM from 'react-dom';

import './FormLogin.css';
import notka_6 from './notka_6.png'


var ws = require('./Websocket.js');

function LoginStatus(props) {
        return <h1>{props.text}</h1>;
}

export class FormLogin extends Component {

  constructor(props) {
    super(props);
    this.state = {login: '', pass: '', init: 0 };

    if (props.login) {
        this.state.login = props.login;
    }
    if (props.pass) {
        this.state.pass = props.pass;
    }

    if (this.state.init) {
        this.state.init++;
    }

    this.handleChange = this.handleChange.bind(this);
    this.handleSubmit = this.handleSubmit.bind(this);
    this.handleRegisterClick = this.handleRegisterClick.bind(this);
    this.handleLoginNowClick = this.handleLoginNowClick.bind(this);
    this.autologin = this.autologin.bind(this);

    const autologin = props.autologin;
    if (autologin) {
        this.autologin(this.props);
    }
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
    var logininput = this.refs.logininput;
    switch (FormLogin.loginState)
    {
        case ws.WsState.LOGIN: {
            logininput.value = "";
            break;
        }
        case ws.WsState.LOGIN_PASS: {
            var newState = {};
            newState['pass'] = this.refs.btnPass.value;
            this.setState(newState);
            //this.state.pass = this.refs.btnPass.value;
            break;
        }

        default: {
            break;
        }
    }
    ws.tx_msg_login(this.state.login, this.state.pass);
  }

  handleRegisterClick(e) {
       e.preventDefault();
       FormLogin.updateLoginState(ws.WsState.REGISTER);
       ReactDOM.render(<div></div>, document.getElementById('headerwrapper'));
       ReactDOM.render(<FormLogin />, document.getElementById('root'));
  }

  handleLoginNowClick(e) {
       e.preventDefault();
       const element = (
                        <div>
                            <div id="header-text"><img src={notka_6} alt="Notka" /></div>
                            online clipboard<br/><br/>
                        </div>
       );
       ReactDOM.render(element, document.getElementById('headerwrapper'));
       FormLogin.updateLoginState(ws.WsState.LOGIN);
       var newState = { login: '', pass: ''};
       this.setState(newState);
       //ReactDOM.render(<FormLogin />, document.getElementById('root'));
  }

  static getLoginState() {
        return FormLogin.loginState;
  }

  static updateLoginState(status) {
        FormLogin.loginState = status;
  }

  autologin(props) {
        this.state.login = props.login;
        this.state.pass = props.pass;
        ws.initWebSocket(props);
        //ws.tx_msg_login(this.state.login, this.state.pass);
  }

  render() {

        let label = null;
	let note = <div></div>;
        if (FormLogin.loginState === ws.WsState.LOGIN ) {
            label = <div>
            <h2>Small cloud storage for your text.</h2>
		<br/>Notka title<br/>
            <input type="text" name="login" onChange={this.handleChange} ref="logininput" />
          </div>;
		note = <div id="register-text">Don't you have any notka yet? You can <a href="" onClick={this.handleRegisterClick}>create one</a>.</div>;
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
                      <input type="password" name="pass" onChange={this.handleChange} ref="btnPass" />
                    </div>;
        } else if (FormLogin.loginState === ws.WsState.LOGGED_IN) {
                    return <LoginStatus text="OK." />;
        } else if (FormLogin.loginState === ws.WsState.LOGIN_FAIL) {
                    var textloginagain = "Wrong password. This notka is protected with different password.";
                    return (
                            <div>
                                <LoginStatus text = {textloginagain} />
                                <div id="loginnow-text"><a href="" onClick={this.handleLoginNowClick}>Try again</a>.</div>
				<div id="register-text">Don't you have any notka yet? You can <a href="" onClick={this.handleRegisterClick}>create one</a>.</div>
                            </div>
                            );
        } else if (FormLogin.loginState === ws.WsState.REGISTER ) {
                    label = <div>
                        <h1>Create your notka</h1><br/>
                        You can protect your text with password or just leave the password empty if you prefer not to use it.
                        <br/>
                        <br/>Notka title<br/>
                        <input type="text" name="login" onChange={this.handleChange} ref="logininput" />
                        <br/>Optional password<br/>
                        <input type="password" name="pass" onChange={this.handleChange} />
                        </div>;
        } else if (FormLogin.loginState === ws.WsState.REGISTERED) {
                    var textlogin = "Perfect! Your notka " + ws.login() + " is ready.";
                    return (
                            <div>
                                <LoginStatus text = {textlogin} />
                                <div id="loginnow-text"><a href="" onClick={this.handleLoginNowClick}>Try it now</a>.</div>
                            </div>
                            );
        } else {
                    return LoginStatus("Sorry, something went wrong... Please try again.");
        }

        return (
            <div id="registerdiv">
                <form onSubmit={this.handleSubmit}>
                {label}
                <input type="submit" value="Go" />
		{note}
                </form>
            </div>
        );
  }
}

