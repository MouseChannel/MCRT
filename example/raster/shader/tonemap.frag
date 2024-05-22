#version 450 core
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#include "Binding.h"
layout(input_attachment_index = 0, binding = e_tonemap_input) uniform subpassInput sceneColor;

layout(location = 0) out vec4 outColor;

void main()
{
    // debugPrintfEXT("Hewe\n");
    vec3 color = subpassLoad(sceneColor).rgb;

    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float mappedLuminance = (luminance * (1.0 + luminance)) / (1.0 + luminance);

    vec3 mappedColor = (mappedLuminance / luminance) * color;

    outColor = vec4(pow(mappedColor, vec3(1.0 / 2.2)), 1.0);
}
