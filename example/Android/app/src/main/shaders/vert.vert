#version 450

vec2 pos[3] = vec2[3](vec2(0.5, 0.5), vec2(-0.5, 0.5), vec2(0, -0.5));
vec3 color[3] = vec3[3](vec3(1., 0., 0.), vec3(0., 1., 0.), vec3(0., 0., 1.));
layout(location = 0) out vec4 out_color;
void main()
{
    // outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    // gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
    gl_Position = vec4(pos[gl_VertexIndex], 0, 1.f);
    out_color = vec4(color[gl_VertexIndex], 1);

    // (0, 0)(-1, -1)(2, 0)(3, -1)(0, 2)(-1, 3)
}
