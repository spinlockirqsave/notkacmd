import React, { Component } from 'react';
import { BrowserRouter as Router, Route} from "react-router-dom";

import { FormLogin } from './FormLogin';

export const AppRouter = () => (
            <Router>
                <div>
                    <Route exact path="/:title/:pass/:more" component={BadUrl} />
                    <Route exact path="/:title/:pass" component={LoginTitlePass} />
                    <Route exact path="/:title" component={LoginTitle} />
                    <Route exact path="/" component={StandardLogin} />
                </div>
            </Router>
);

const BadUrl = ({ match }) => (
        <div>
            Bad url.<br/><br/>
            API usage:<br/><br/>
            notka.online/title  - to get notka which is not protected with password<br/>
            notka.online/title/password - to get notka which is protected with password
        </div>
);

const LoginTitlePass = ({ match }) => (
        <div>
            <FormLogin login={match.params.title} pass={match.params.pass} autologin='true' />
        </div>
);

const LoginTitle = ({ match }) => (
    <div>
        <FormLogin login={match.params.title} pass="" autologin='true' />
    </div>
);

const StandardLogin = ({ match }) => (
    <div>
        <FormLogin login="" pass=""/>
    </div>
);
