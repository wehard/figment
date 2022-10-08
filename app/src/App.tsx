import { useState, useEffect, useRef } from "react";
import Renderer from "./Renderer";
import "./App.css";

function App() {
  return (
    <div className="App">
      <h1>Figment</h1>
      <Renderer></Renderer>
    </div>
  );
}

export default App;
