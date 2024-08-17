#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform constants
{
    mat4 model;
} pc;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 fragCol;

void main()
{
    gl_Position = ubo.proj * ubo.view * pc.model * vec4(pos, 1.0);
    fragCol = col;
}
