import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

type CanvasContext = {
  onCanvasResize: (w: number, h: number) => void;
  insertObject: () => void;
};

type ModuleDesc = {
  canvas: HTMLCanvasElement | null;
  arguments: string[];
};

interface RendererProps {
  width: number;
  height: number;
  registerCallback: (ctx: CanvasContext) => void;
}

const Renderer = (props: RendererProps) => {
  const [context, setContext] = useState<CanvasContext>();
  const canvas = useRef<HTMLCanvasElement>(null);

  const initialize = async (module: ModuleDesc) => {
    const rendererContext = await loadRenderer(module);
    const ctx = {
      onCanvasResize: rendererContext._onCanvasResize,
      insertObject: rendererContext._insertObject,
    };
    props.registerCallback(ctx);
    setContext(ctx);
  };

  useEffect(() => {
    const module = {
      canvas: canvas.current,
      arguments: [props.width.toString(), props.height.toString()],
    };

    initialize(module).catch(console.error);
  }, []);

  return (
    <div className=''>
      <canvas
        className='webgl-canvas'
        id='canvas'
        ref={canvas}
        width={props.width}
        height={props.height}
        onContextMenu={(e) => e.preventDefault()}
      ></canvas>
    </div>
  );
};

export default Renderer;
