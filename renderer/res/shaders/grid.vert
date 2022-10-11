#version 300 es

layout(location = 0) in vec4 position;

uniform vec4 obj_color;
uniform vec2 pitch;

out vec4 o_col;
out vec2 o_pitch;

void main()
{
	o_col = obj_color;
	o_pitch = pitch;
	gl_Position = vec4(position.xyz, 1.0);
}
