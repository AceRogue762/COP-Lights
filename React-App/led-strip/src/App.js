/**
 * Entrypoint for the LED strip client application. ReactDOM
 * rendering included to allow webpack to compress the app into
 * pure html/javascript.
 */

import React, { Component } from "react";
import ReactDOM from "react-dom";

// Import components
import MainAppBar from "./js/components/MainAppBar";
import AnimationButtonList from "./js/components/AnimationButtonList";
import EffectPicker from "./js/components/EffectPicker";


class App extends Component {
  render() {
    return (
    //  <div className="App">
    //    <MainAppBar />
    //    <div style={{marginTop: 20}}>
    //        <AnimationButtonList />
    //        <EffectPicker />
    //    </div>
    //  </div>
    <div className="App">
      <MainAppBar />
    </div>
    );
  }
}

export default App;

const wrapper = document.getElementById("container");
wrapper ? ReactDOM.render(<App />, wrapper) : false;
