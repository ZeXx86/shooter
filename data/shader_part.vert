#version 400 core

struct LightInfo {
	vec4 position;		//Light position   X,Y,Z, directional=0 OR positional=1
	vec4 ambient;		//Light ambient color
	vec4 diffuse;		//Light diffuse color
	vec4 specular;		//Light specular color
	vec4 spot_dir;		//Specifies light direction
	int spot_exp;		//Specifies intensity distribution of spotlight
	int spot_cut;		//Specifies maximum spread angle of spotlight (180 = off).
};

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in float PartLife;

uniform mat4 PMatrix;		//Camera projection matrix
uniform mat4 VMatrix;		//Camera view matrix
uniform mat4 MVMatrix;		//VMatrix * Model matrix
uniform mat3 NormalMatrix;	//MVMatrix ... -> converted into normal matrix (inverse transpose operation)
uniform vec3 CameraPos;

out float ecPartLife;
out vec4 ecPosition;

void main()
{
    float dist = distance (CameraPos, VertexPosition);

    ecPosition = MVMatrix * vec4 (VertexPosition, 1.0);
    ecPartLife = PartLife;

    gl_PointSize = 64 / dist;
    gl_Position  = PMatrix * ecPosition;
}
