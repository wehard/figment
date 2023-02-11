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
  onChange: (v: string) => void;
}

const fragSource = `#version 300 es
precision mediump float;
        
uniform vec4 obj_color;
in vec4 o_col;
out vec4 color;
        
void main()
{
  color = o_col;
}
`;

const CodeEditor = (props: CodeEditorProps) => {
  const handleEditorChange = (value: string | undefined, ev: any) => {
    props.onChange(value ?? '');
  };

  return (
    <div className='h-[30rem] w-[60rem]'>
      <Editor
        defaultLanguage='glsl'
        defaultValue='// some comment'
        value={fragSource}
        theme={'vs-dark'}
        onChange={handleEditorChange}
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
  const [shaderSource, setShaderSource] = useState<string>('');

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

  const sendShaderString = (s: string) => {
    // Create a pointer using the 'Glue' method and the String value
    // var ptr  = allocate(intArrayFromString(myStrValue), 'i8', ALLOC_NORMAL);
    // Call the method passing the pointer
    // val retPtr = _hello(ptr);
    // Retransform back your pointer to string using 'Glue' method
    // var resValue = Pointer_stringify(retPtr);
    // Free the memory allocated by 'allocate'
    // _free(ptr);
  };

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
        <div className='fixed bottom-0 right-0 w-[60rem] border-neutral-900 bg-neutral-900'>
          <div className='flex items-start space-x-1'>
            <button
              className='w-12 bg-neutral-500 active:bg-neutral-700'
              onClick={() => setShowEditor(!showEditor)}
            >
              <span>{showEditor ? 'v' : '^'}</span>
            </button>
            <button
              className='w-12 bg-neutral-500 active:bg-neutral-700'
              onClick={() => canvasContext?.updateShader(shaderSource)}
            >
              S
            </button>
          </div>
          {showEditor && <CodeEditor onChange={setShaderSource} />}
        </div>
      </div>
    </div>
  );
}

export default App;
