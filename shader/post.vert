#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
layout(location = 0) in vec3 inposition;
layout(location = 1) in vec2 inUV;
layout(location = 1) out vec2 outUV;
void main()
{
    // debugPrintfEXT("message:%d  %f %f %f \n", gl_VertexIndex, inposition.x, inposition.y, inposition.z);
    gl_Position = vec4(inposition, 1.);
    outUV = inUV;

}