#version 300 es
precision mediump float;

in vec2 o_pitch;
in vec4 o_col;
out vec4 color;

void main()
{    
    if (int(mod(gl_FragCoord.x, o_pitch[0])) == 0 ||
        int(mod(gl_FragCoord.y, o_pitch[1])) == 0) {
        color = o_col;
    } else {
        color = vec4(0.0,0.0,0.0,1.0);
    }
}
