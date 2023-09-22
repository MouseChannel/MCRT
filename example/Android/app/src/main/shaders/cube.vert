#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_nrm;
layout(location = 2) in vec3 in_color;
// layout(location = e_texture_index) in int in_texture_index;
layout(location = 3) in vec2 in_texCoord;

layout(location = 1) out vec2 outUV;

layout(binding = 0) uniform UniformBuffer
{
    mat4 project;
    mat4 view;
    mat4 model;
}
ubo;
layout(push_constant) uniform PushConstant
{
    mat4 model;
}
pc;
void main()
{
    gl_Position = ubo.project * ubo.view * pc.model * vec4(in_pos, 1);

    outUV = in_texCoord;
}