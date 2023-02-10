import { useState, useEffect, useRef } from 'react';
import Editor from '@monaco-editor/react';
import Renderer, { CanvasContext } from './Renderer';
import './App.css';

interface ToolbarProps {
  canvasContext: CanvasContext;
}

const Toolbar = (props: ToolbarProps) => {
  const canvasContext = props.canvasContext;
  const [inputEnabled, setInputEnabled] = useState<boolean>(true);

  return (
    <div className='flex h-12 w-full border-b border-neutral-700 bg-neutral-800'>
      <button
        className='w-12 active:bg-neutral-700'
        onClick={() => {
          if (canvasContext) canvasContext.insertObject(1);
        }}
      >
        1
      </button>
      <button
        className='w-12 active:bg-neutral-700'
        onClick={() => {
          if (canvasContext) canvasContext.insertObject(2);
        }}
      >
        2
      </button>
      <button className='w-12 active:bg-neutral-700'>3</button>
      <button className='w-12 active:bg-neutral-700'>4</button>
      <button className='w-12 active:bg-neutral-700'>5</button>
      <button className='w-12 active:bg-neutral-700'>6</button>
      <button className='w-12 active:bg-neutral-700'>7</button>
      <button
        className='w-12 active:bg-neutral-700'
        onClick={() => {
          if (canvasContext) {
            const i = inputEnabled ? 0 : 1;
            setInputEnabled(!inputEnabled);
            canvasContext.setInputEnabled(i);
          }
        }}
      >
        8
      </button>
      <span>
        {'Input:'} {inputEnabled ? 'true' : 'false'}
      </span>
    </div>
  );
};

const debounce = (func: () => void, ms: number) => {
  let timer: number | undefined;
  return () => {
    clearTimeout(timer);
    timer = setTimeout(() => {
      timer = undefined;
      func.apply(this, []);
    }, ms);
  };
};

function App() {
  const renderDivRef = useRef<HTMLDivElement>(null);
  const [canvasContext, setCanvasContext] = useState<CanvasContext>();
  const [size, setSize] = useState<{ width: number; height: number }>({ width: 320, height: 200 });

  const handleEditorChange = (value: string | undefined, ev: any) => {
    console.log(value);
  };

  useEffect(() => {
    const debounceHandleResize = debounce(() => {
      if (renderDivRef.current)
        canvasContext?.onCanvasResize(
          renderDivRef.current.clientWidth,
          renderDivRef.current.clientHeight
        );
    }, 10);

    window.addEventListener('resize', debounceHandleResize);
    return () => {
      window.removeEventListener('resize', debounceHandleResize);
    };
  });

  return (
    <div id='app' className='app h-screen w-screen overflow-x-hidden overflow-y-hidden bg-black'>
      {canvasContext && <Toolbar canvasContext={canvasContext}></Toolbar>}
      <div className='flex h-full flex-row '>
        <div className='flex h-full w-[15rem] min-w-[15rem] flex-col content-start items-start justify-start border-r border-neutral-700 bg-neutral-800 p-2'>
          <ul className='flex list-inside list-decimal flex-col flex-wrap content-start items-start justify-start justify-items-start'>
            <li>Insert quad</li>
            <li>Insert circle</li>
          </ul>
        </div>
        <div ref={renderDivRef} className={'h-full w-full'}>
          <Renderer
            initialWidth={renderDivRef.current?.clientWidth ?? 1280}
            initialHeight={renderDivRef.current?.clientHeight ?? 720}
            registerCallback={setCanvasContext}
          />
        </div>
        {/* <div className='fixed bottom-0 h-[30rem] w-[60rem] border-t-4 border-r-4 border-neutral-700'>
          <Editor
            defaultLanguage='glsl'
            defaultValue='// some comment'
            value='code'
            theme={'vs-dark'}
            onChange={handleEditorChange}
          />
        </div> */}
      </div>
    </div>
  );
}

export default App;
