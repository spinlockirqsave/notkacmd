import React, { Component } from 'react';


class HelloGreeting extends Component {
              render() {
                  return (
                     <div>
                      <h1>Hello!</h1>
                      <h2>It is {new Date().toLocaleTimeString()}.</h2>
                    </div>
                  );
              }
          }

export default HelloGreeting;
