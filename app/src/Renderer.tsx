import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

const Renderer = (props: { width: number; height: number }) => {
  const [context, setContext] = useState<any>(null);
  const canvas = useRef<HTMLCanvasElement>(null);

  const handleResize = (ev: UIEvent) => {
    if (context) {
      context.canvas.width = window.innerWidth;
      context.canvas.height = window.innerHeight;
      context._onCanvasResize(window.innerWidth, window.innerHeight);
    }
  };

  useEffect(() => {
    if (context !== null) {
      console.log('Renderer initialized!');
    }
    const initWidth = props.width; // window.innerWidth;
    const initHeight = props.height; //window.innerHeight;
    let Module = {
      canvas: canvas.current,
      arguments: [initWidth.toString(), initHeight?.toString()],
    };
    loadRenderer(Module).then((rendererContext: any) => {
      setContext(rendererContext);
      // window.addEventListener('resize', handleResize);
    });
    return () => {
      // window.removeEventListener('resize', handleResize);
    };
  }, []);

  return (
    <div className=''>
      <canvas
        className='webgl-canvas'
        id='canvas'
        ref={canvas}
        width={100}
        height={100}
        onContextMenu={(e) => e.preventDefault()}
      ></canvas>
    </div>
  );
};

export default Renderer;
