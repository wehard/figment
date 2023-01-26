import { useState, useEffect, useRef } from 'react';
import Renderer from './Renderer';
import './App.css';

function App() {
  const rref = useRef<HTMLDivElement>(null);
  const [size, setSize] = useState<{ width: number; height: number }>({
    width: 1280,
    height: 400,
  });

  useEffect(() => {
    if (rref.current) {
      const s = getComputedStyle(rref.current);
      setSize({ width: parseInt(s.width), height: parseInt(s.height) });
    }
  }, []);

  return (
    <div>
      <div className='flex h-12 w-full border-b border-neutral-700 bg-neutral-800'>
        <button>Insert</button>
      </div>
      <div className='flex h-full grow flex-row'>
        <div className='w-2/12 grow-0 border-r border-neutral-700 bg-neutral-800'>HELLO</div>
        <div ref={rref} className='grow bg-red-400'>
          <Renderer width={size.width} height={size.height} />
        </div>
        <div className='w-2/12 grow-0  border-l border-neutral-700 bg-neutral-800'>WORLD</div>
      </div>
    </div>
  );
}

export default App;
