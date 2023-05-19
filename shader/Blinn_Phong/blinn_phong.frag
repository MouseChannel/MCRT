#version 460
// #extension GL_KHR_vulkan_glsl:enable
// #extension GL_EXT_ray_tracing : require
// #extension GL_EXT_ray_tracing:require
layout(location = 0) out vec4 outColor;
// layout(location=0)in vec2 Texcoord;
layout(location = 1) in vec2 inUV;
layout(set = 0, binding = 1) uniform UBO
{
    vec3 color;
}
ubo;
layout(set = 0, binding = 2) uniform sampler2D Sampler;
void main()
{

    // gl_VertexIndex = i;

    // vec4(ubo.color,1)*
    // outColor = texture(Sampler, inUV);
    outColor = vec4(1, 0, 0, 1);
    // gl_
}
