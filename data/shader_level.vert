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
layout(location = 1) in vec2 VertexTexCoord;
layout(location = 2) in vec3 VertexNormal;

uniform mat4 PMatrix;		//Camera projection matrix
uniform mat4 VMatrix;		//Camera view matrix
uniform mat4 MVMatrix;		//VMatrix * Model matrix
uniform mat3 NormalMatrix;	//MVMatrix ... -> converted into normal matrix (inverse transpose operation)

uniform LightInfo light;

//The prefix ec means Eye Coordinates in the Eye Coordinate System
out vec4 ecPosition;			
out vec3 ecLightDir;
out vec3 ecNormal;
out vec3 ecViewDir;
// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 ShadowCoord;

uniform mat4 DepthBiasMVP;

void main()
{
	ecPosition = MVMatrix * vec4(VertexPosition, 1.0);
	ecLightDir   = vec3(VMatrix * light.position - ecPosition);
	ecNormal =  NormalMatrix * VertexNormal;
	ecViewDir = -vec3(ecPosition);
	UV = VertexTexCoord;
	gl_Position  = PMatrix * ecPosition;
	
	ShadowCoord = DepthBiasMVP * gl_Position;
}
