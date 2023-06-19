#version 330 core
precision mediump float;

out vec4 col;

in vec2 f_uv;

uniform sampler2D u_Texture;

void main()
{
	col = texture(u_Texture, f_uv);
}
