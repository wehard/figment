#version 300 es
precision mediump float;

uniform vec4 obj_color;
in vec4 o_col;
out vec4 color;

void main()
{
  color = o_col;
}
