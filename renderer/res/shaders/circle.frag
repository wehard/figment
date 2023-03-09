#version 300 es
precision mediump float;

uniform vec4 obj_color;
uniform int obj_id;
in vec4 o_col;
in vec3 v_pos;

layout(location = 0) out vec4 color;
layout(location = 1) out int id;

float simple_random(float seed)
{
    return fract(sin(seed) * 43758.5453123);
}

vec3 random_color(float seed)
{
    float r = simple_random(seed);
    float g = simple_random(seed + 1.0);
    float b = simple_random(seed + 2.0);
    
    return vec3(r, g, b);
}

void main()
{
  float edge_smooth = 0.0;
  float width = 1.0;
  float distance = 1.0 - length(v_pos);
  float inside = smoothstep(0.0, edge_smooth, distance);
  inside *= smoothstep(width + edge_smooth, width, distance);

	if (inside == 0.0)
		discard;

  color = vec4(random_color(float(obj_id + 42)), 1.0);
  color.a = inside;
  id = obj_id;
}
