#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_vel;
layout(location = 2) in float v_life;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform vec3 m_pos;

out float life;
out vec3 velocity;
out float m_dist;

void main()
{
	life = v_life;
	velocity = v_vel;
	vec4 p = proj_matrix * view_matrix * vec4(v_pos, 1.0);
	vec4 m = proj_matrix * view_matrix * vec4(m_pos, 1.0);

	m_dist = length(m - p);

	gl_Position = p;
}
