#version 300 es
precision mediump float;

uniform vec4 obj_color;
in vec4 o_col;
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 id;

void main()
{
  color = o_col;
  id = vec4(1.0, 0.0, 0.0, 1.0);
}
