#version 460
#extension GL_EXT_debug_printf : enable
vec3[3] positions = vec3[3](vec3(1.0, 1.0, 0.0), vec3(1.0, -3.0, 0.0), vec3(-3.0, 1.0, 0.0));

void main()
{

 
    gl_Position = vec4(positions[gl_VertexIndex], 1);
 
}
