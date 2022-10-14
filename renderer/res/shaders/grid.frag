#version 300 es
precision mediump float;

uniform vec2 offset;
uniform vec2 pitch;

in vec4 o_col;
out vec4 color;

void main()
{
	float ox = (gl_FragCoord.x) + offset.x;
	float oy = (gl_FragCoord.y) + offset.y;
	if (int(mod(ox, pitch.x)) == 0 ||
			int(mod(oy, pitch.y)) == 0) {
			color = o_col;
	} else {
			discard;
	}
}
