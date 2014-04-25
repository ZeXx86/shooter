#version 400 core

//precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;
layout(location = 2) in vec3 VertexNormal;

uniform mat4 depthMVP;

void main()
{
    gl_Position  = depthMVP * vec4 (VertexPosition, 1.0);
}
