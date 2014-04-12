#version 400 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

uniform mat4 PMatrix;		//Camera projection matrix
uniform mat4 VMatrix;		//Camera view matrix
uniform mat4 MVMatrix;		//VMatrix * Model matrix
uniform mat3 NormalMatrix;	//MVMatrix ... -> converted into normal matrix (inverse transpose operation)

//The prefix ec means Eye Coordinates in the Eye Coordinate System
out vec4 ecPosition;			
out vec3 ecLightDir;
out vec3 ecNormal;
out vec3 ecViewDir;
// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main()
{
    ecPosition = vec4(VertexPosition, 1.0);
    UV = VertexTexCoord;
    gl_Position  = PMatrix * ecPosition;
}
