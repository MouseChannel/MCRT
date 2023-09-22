#version 450

layout(location=0)out vec4 outColor;
// layout(location=0)in vec2 Texcoord;
layout(location=1)in vec2 inUV;
layout(binding=1)uniform UBO{
    vec3 color;
}ubo;
layout(binding=2)uniform sampler2D Sampler;
void main(){
    outColor=
    // vec4(ubo.color,1)*
    texture(Sampler,inUV);;
}
