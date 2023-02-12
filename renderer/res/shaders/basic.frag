#version 300 es
precision mediump float;

uniform vec4 obj_color;
in vec4 o_col;

layout(location = 0) out vec4 color;
layout(location = 1) out uint id;

void main()
{
  color = o_col;
  id = uint(50);
}
