import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import loadRenderer from './renderer/index.js';

export type CanvasContext = {
  onCanvasResize: (w: number, h: number) => void;
  insertObject: (n: number) => void;
  setInputEnabled: (n: number) => void;
  updateShader: (vert: string, frag: string) => void;
  allocate?: (buffer: number[], t: string) => number;
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
    const ctx: CanvasContext = {
      onCanvasResize: rendererContext._onCanvasResize,
      insertObject: rendererContext._insertObject,
      setInputEnabled: rendererContext._setInputEnabled,
      updateShader: (vert: string, frag: string) => {
        var vertPtr = rendererContext.allocate(
          rendererContext.intArrayFromString(vert),
          rendererContext.ALLOC_NORMAL
        );
        var fragPtr = rendererContext.allocate(
          rendererContext.intArrayFromString(frag),
          rendererContext.ALLOC_NORMAL
        );
        rendererContext._updateShader(vertPtr, fragPtr);
        // rendererContext._free(ptr);
      },
    };

    rendererContext.doNotCaptureKeyboard = true;
    console.log(rendererContext);

    props.registerCallback(ctx);
    setContext(ctx);
  };

  useEffect(() => {
    const module = {
      canvas: canvas.current,
      arguments: [props.initialWidth.toString(), props.initialHeight.toString()],
      doNotCaptureKeyboard: false,
      preRun: [],
    };

    initialize(module).catch(console.error);
  }, []);

  return (
    <canvas
      className='webgl-canvas'
      id='canvas'
      ref={canvas}
      width={props.initialWidth}
      height={props.initialHeight}
      onContextMenu={(e) => e.preventDefault()}
    ></canvas>
  );
};

export default Renderer;
