import { useState, useEffect, useRef } from 'react';
import Renderer from './Renderer';
import './App.css';

function App() {
  const rref = useRef<HTMLDivElement>(null);
  const [size, setSize] = useState<{ width: number; height: number }>({
    width: 1280,
    height: 720,
  });

  useEffect(() => {
    if (rref.current) {
      const s = getComputedStyle(rref.current);
      setSize({ width: parseInt(s.width), height: parseInt(s.height) });
      console.log('helo');
    }
  }, [rref.current]);

  return (
    <div className='h-screen overflow-y-hidden'>
      <div className='flex h-12 w-full border-b border-neutral-700 bg-neutral-800'>
        <button className='w-12 active:bg-neutral-700'>1</button>
        <button className='w-12 active:bg-neutral-700'>2</button>
        <button className='w-12 active:bg-neutral-700'>3</button>
        <button className='w-12 active:bg-neutral-700'>4</button>
        <button className='w-12 active:bg-neutral-700'>5</button>
        <button className='w-12 active:bg-neutral-700'>6</button>
        <button className='w-12 active:bg-neutral-700'>7</button>
        <button className='w-12 active:bg-neutral-700'>8</button>
      </div>
      <div className='flex h-full grow flex-row opacity-70'>
        <div className='grow-1 flex w-96 border-r border-neutral-700 bg-neutral-800 p-2'>
          Hello, world!
        </div>
        <div className='grow'>
          <Renderer width={size.width} height={size.height} />
        </div>
      </div>
    </div>
  );
}

export default App;
