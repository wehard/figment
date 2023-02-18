#version 300 es
precision mediump float;

uniform vec4 obj_color;
uniform int obj_id;
in vec4 o_col;
in vec3 v_pos;

layout(location = 0) out vec4 color;
layout(location = 1) out int id;

void main()
{
  float edge_smooth = 0.0;
  float width = 0.05;
  float distance = 1.0 - length(v_pos);
  float inside = smoothstep(0.0, edge_smooth, distance);
  inside *= smoothstep(width + edge_smooth, width, distance);

	if (inside == 0.0)
		discard;

  color = o_col;
  color.a = inside;
  id = obj_id;
}
