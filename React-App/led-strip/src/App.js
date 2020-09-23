/**
 * Entrypoint for the LED strip client application. ReactDOM
 * rendering included to allow webpack to compress the app into
 * pure html/javascript.
 */

import React, { Component } from "react";
import ReactDOM from "react-dom";

// Import components
import MenuContent from "./js/components/MenuContent";


class App extends Component {
  render() {
    return (
    <div className="App">
      <MenuContent />
    </div>
    );
  }
}

export default App;

const wrapper = document.getElementById("container");
wrapper ? ReactDOM.render(<App />, wrapper) : false;
