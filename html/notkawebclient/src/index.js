/*
 * index.js
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


import React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter as Router, Route} from "react-router-dom";
import './index.css';

import { FormLogin, ParamsRoute} from './FormLogin';
import registerServiceWorker from './registerServiceWorker';
import { AppRouter } from './AppRouter';


            ReactDOM.render((
              <Router>
                <AppRouter />
              </Router>
            ), document.getElementById('root'));

//ReactDOM.render(<FormLogin />, document.getElementById('root'));

registerServiceWorker();

var ws = require('./Websocket.js');
ws.initWebSocket(null);
