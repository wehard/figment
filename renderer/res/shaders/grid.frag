#version 300 es
precision mediump float;

uniform vec2 offset;

in vec2 o_pitch;
in vec4 o_col;
out vec4 color;

void main()
{    
    if (int(mod(gl_FragCoord.x + offset[0]*1280.0*0.28, o_pitch[0])) == 0 ||
        int(mod(gl_FragCoord.y + offset[1]*720.0*0.5, o_pitch[1])) == 0) {
        color = o_col;
    } else {
        discard;
    }
}
