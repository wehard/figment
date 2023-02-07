import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

export type CanvasContext = {
  onCanvasResize: (w: number, h: number) => void;
  insertObject: () => void;
};

type ModuleDesc = {
  canvas: HTMLCanvasElement | null;
  arguments: string[];
};

interface RendererProps {
  initialWidth: number;
  initialHeight: number;
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

  const handleResize = (e: Event) => {
    console.log('resize', context);
    context?.onCanvasResize(1280, 720);
  };

  useEffect(() => {
    const module = {
      canvas: canvas.current,
      arguments: [props.initialWidth.toString(), props.initialHeight.toString()],
    };

    initialize(module).catch(console.error);
    window.addEventListener('resize', handleResize);
    return () => {
      window.removeEventListener('resize', handleResize);
    };
  }, [props.initialWidth, props.initialHeight]);

  return (
    <div className=''>
      <canvas
        className='webgl-canvas'
        id='canvas'
        ref={canvas}
        width={props.initialWidth}
        height={props.initialHeight}
        onContextMenu={(e) => e.preventDefault()}
      ></canvas>
    </div>
  );
};

export default Renderer;
