import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

const Renderer = () => {
  const [context, setContext] = useState(null);
  const canvas = useRef(null);

  useEffect(() => {
    if (context !== null) {
      console.log('Renderer initialized!');
    }
    let Module = { canvas: canvas.current };
    loadRenderer(Module).then((rendererContext: any) => {
      console.log(rendererContext);
      setContext(rendererContext);
    });
  }, []);

  return (
    <>
      <canvas id='canvas' ref={canvas} onContextMenu={(e) => e.preventDefault()}></canvas>
    </>
  );
};

export default Renderer;
