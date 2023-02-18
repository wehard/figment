#version 300 es

layout(location = 0) in vec4 position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform vec4 obj_color;

out vec4 o_col;
out vec3 v_pos;

void main()
{
	o_col = obj_color;
	v_pos = position.xyz;
	gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position.xyz, 1.0);
}
