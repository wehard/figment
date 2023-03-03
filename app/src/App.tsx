import { useState, useEffect, useRef } from 'react';
import Editor from '@monaco-editor/react';
import Renderer, { CanvasContext } from './Renderer';
import './App.css';

interface ToolbarProps {
  canvasContext: CanvasContext;
  toggleShowEditor: () => void;
}

const Toolbar = (props: ToolbarProps) => {
  const canvasContext = props.canvasContext;

  return (
    <div className='flex h-full w-12 flex-col border-b border-neutral-700 bg-neutral-900'>
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
      <button
        className='w-12 active:bg-neutral-700'
        onClick={() => {
          if (canvasContext) canvasContext.insertObject(3);
        }}
      >
        3
      </button>
      <button className='w-12 active:bg-neutral-700'>4</button>
      <button className='w-12 active:bg-neutral-700'>5</button>
      <button className='w-12 active:bg-neutral-700'>6</button>
      <button className='w-12 active:bg-neutral-700'>7</button>
      <button className='w-12 active:bg-neutral-700' onClick={props.toggleShowEditor}>
        E
      </button>
    </div>
  );
};

interface CodeEditorProps {
  value: string;
  onChange: (v: string) => void;
  onLoad: (v: string) => void;
}

const CodeEditor = (props: CodeEditorProps) => {
  const handleEditorChange = (
    value: string | undefined,
    ev: monaco.editor.IModelContentChangedEvent
  ) => {
    props.onChange(value ?? '');
  };

  return (
    <div className='h-[30rem] w-full'>
      <Editor
        defaultLanguage='glsl'
        defaultValue={props.value}
        value={props.value}
        theme={'vs-dark'}
        onChange={handleEditorChange}
        onMount={() => props.onLoad(props.value)}
      />
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
  const [showEditor, setShowEditor] = useState<boolean>(true);
  const [vertShaderSource, setVertShaderSource] = useState<string>();
  const [fragShaderSource, setFragShaderSource] = useState<string>();

  useEffect(() => {
    const debounceHandleResize = debounce(() => {
      if (renderDivRef.current)
        canvasContext?.onCanvasResize(
          renderDivRef.current.clientWidth,
          renderDivRef.current.clientHeight
        );
    }, 10);

    debounceHandleResize();
    window.addEventListener('resize', debounceHandleResize);
    return () => {
      window.removeEventListener('resize', debounceHandleResize);
    };
  });

  return (
    <div id='app' className='app h-screen w-screen overflow-x-hidden  bg-black'>
      <div className='flex h-full w-full flex-row '>
        {canvasContext && (
          <Toolbar
            canvasContext={canvasContext}
            toggleShowEditor={() => {
              setShowEditor(!showEditor);
            }}
          />
        )}
        <div ref={renderDivRef} className={'h-full w-full'}>
          <Renderer
            initialWidth={renderDivRef.current?.clientWidth ?? 1280}
            initialHeight={renderDivRef.current?.clientHeight ?? 720}
            registerCallback={setCanvasContext}
          />
        </div>
        <div className='fixed bottom-0 right-0 block w-[90rem] rounded-tl-lg border-neutral-900 bg-neutral-900'>
          <div className='flex items-start space-x-1 rounded-tl-lg'>
            <button
              className='w-12 rounded-tl-lg bg-neutral-500 hover:rounded-tl-lg active:bg-neutral-700'
              onClick={() => setShowEditor(!showEditor)}
            >
              <span>{showEditor ? 'v' : '^'}</span>
            </button>
            <button
              className='w-12 bg-neutral-500 active:bg-neutral-700'
              onClick={() => {
                if (vertShaderSource && fragShaderSource) {
                  console.log('shader sources:', vertShaderSource, fragShaderSource);
                  canvasContext?.updateShader(vertShaderSource, fragShaderSource);
                }
              }}
            >
              S
            </button>
            <button
              className='w-12 bg-neutral-500 active:bg-neutral-700'
              onClick={() => {
                setVertShaderSource(vertShaderSource);
                setFragShaderSource(fragShaderSource);
              }}
            >
              R
            </button>
          </div>
          <div className='flex flex-row'>
            {showEditor && canvasContext && (
              <CodeEditor
                value={canvasContext?.defaultVertSource}
                onChange={setVertShaderSource}
                onLoad={setVertShaderSource}
              />
            )}
            {showEditor && canvasContext && (
              <CodeEditor
                value={canvasContext.defaultFragSource}
                onChange={setFragShaderSource}
                onLoad={setFragShaderSource}
              />
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
