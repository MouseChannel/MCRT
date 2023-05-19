
#version 460

layout(location = 0) in vec3 Position;

layout(location = 2) in vec2 inUV;

layout(location = 1) out vec2 outUV;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 project;
    mat4 view;
    mat4 model;
}
ubo;

void sss()
{
}
void main()
{
    gl_Position = ubo.project * ubo.view * ubo.model * vec4(Position, 1);
    sss();

    outUV = inUV;
}