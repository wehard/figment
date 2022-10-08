import { useState, useEffect, useRef } from 'react';
import Renderer from './Renderer';
import './App.css';

function App() {
  return (
    <div className='flex flex-col justify-center items-center space-y-2'>
      <h1 className='text-white'>Figment</h1>
      <div className='outline-dashed outline-white rounded-md'>
        <Renderer />
      </div>
      <button>Insert</button>
    </div>
  );
}

export default App;
