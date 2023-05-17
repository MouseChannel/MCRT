#version 460

#extension GL_EXT_debug_printf : enable
// layout(location = 2) in vec4 incolor;
layout(location = 1) in vec2 inUV;
layout(location = 0) out vec4 outColor;
layout(binding = 0) uniform sampler2D Sampler;
layout(set = 0, binding = 1, rgba32f) uniform image2D image;
void main()
{

    vec4 color = imageLoad(image, ivec2(100));
    // if (inUV.x < 1e5 && inUV.y < 1e5)
    //     debugPrintfEXT("message from frag %f %f %f %f\n", color.x, color.y, color.z, color.w);
    outColor = texture(Sampler, inUV);
}