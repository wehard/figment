import { useState, useEffect, useRef } from 'react';
import Renderer from './Renderer';
import './App.css';

function App() {
  return (
    <div>
      <div className='flex h-12 w-full border-b border-neutral-700 bg-neutral-800'>
        <button>Insert</button>
      </div>
      <div className='flex h-screen flex-row'>
        <div className='flex w-2/12 border-r border-neutral-700 bg-neutral-800'></div>
        <div>
          <Renderer />
        </div>
        <div className='flex w-2/12 border-l border-neutral-700 bg-neutral-800'></div>
      </div>
    </div>
  );
}

export default App;
