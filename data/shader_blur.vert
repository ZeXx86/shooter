#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

uniform mat4 PMatrix;		

out vec2 UV;

void main()
{
    UV = VertexTexCoord;
    gl_Position  = PMatrix * vec4(VertexPosition, 1.0);
}
