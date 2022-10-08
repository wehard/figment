import React, { useEffect, useRef, useState } from 'react';

// @ts-ignore
import loadRenderer from './renderer/index.js';

const Renderer = () => {
  const [context, setContext] = useState(undefined);
  const canvas = useRef(null);

  useEffect(() => {
    let Module = { canvas: canvas.current };
    loadRenderer(Module).then((result: any) => {
      console.log(result);
      setContext(result);
    });
  }, []);

  return (
    <>
      <canvas id='canvas' ref={canvas}></canvas>
    </>
  );
};

export default Renderer;
