import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

export type CanvasContext = {
  onCanvasResize: (w: number, h: number) => void;
  insertObject: (n: number) => void;
};

type ModuleDesc = {
  canvas: HTMLCanvasElement | null;
  arguments: string[];
  preRun: (() => void)[];
  doNotCaptureKeyboard: boolean;
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

    rendererContext.doNotCaptureKeyboard = true;
    console.log(rendererContext);

    props.registerCallback(ctx);
    setContext(ctx);
    window.addEventListener('resize', handleResize);
  };

  const handleResize = (e: UIEvent) => {
    console.log('resize', context);

    context?.onCanvasResize(1280, 720);
  };

  useEffect(() => {
    const module = {
      canvas: canvas.current,
      arguments: [props.initialWidth.toString(), props.initialHeight.toString()],
      doNotCaptureKeyboard: true,
      preRun: [
        function () {
          const SDL_EMSCRIPTEN_KEYBOARD_ELEMENT = '...';
        },
      ],
    };

    initialize(module).catch(console.error);
    return () => {
      window.removeEventListener('resize', handleResize);
    };
  }, []);

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
