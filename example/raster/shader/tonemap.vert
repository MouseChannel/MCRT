#version 460
#extension GL_EXT_debug_printf : enable
vec3[3] positions = vec3[3](vec3(1.0, 1.0, 0.0), vec3(1.0, -3.0, 0.0), vec3(-3.0, 1.0, 0.0));

void main()
{

    gl_Position = vec4(positions[gl_VertexIndex], 1);

    // vec2 outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    // gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}
