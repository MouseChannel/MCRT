#version 460
// layout(location = 2) in vec4 incolor;
layout(location = 1) in vec2 inUV;
layout(location = 0) out vec4 outColor;
layout( binding = 0) uniform sampler2D Sampler;
void main()
{
 
    outColor = texture(Sampler, inUV);
   
}