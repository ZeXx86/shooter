#version 400 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D TexSampler;
uniform float life;

in vec2 UV;

void main()
{
   FragColor =  vec4(1.0,0.0,0.0,life)*vec4(texture (TexSampler, UV).rgb,1.0);
}
