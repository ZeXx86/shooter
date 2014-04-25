#version 400 core

//precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4 (1.0, 1.0, 1.0, 1.0);
}
