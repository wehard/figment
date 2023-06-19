#version 330 core

layout(location = 0) in vec4 position;

uniform vec4 obj_color;

out vec4 o_col;

void main()
{
	o_col = obj_color;
	gl_Position = vec4(position.xyz, 1.0);
}
