#version 400 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D TexSampler;

in vec2 UV;

void main()
{
   FragColor =  vec4(texture (TexSampler, UV).rgb,1.0);
   //FragColor =  vec4(0.5f, 0.5f,0.5f,0.8f);
}
