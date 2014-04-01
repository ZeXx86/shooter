#version 400 core

//precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

struct LightInfo {
	vec4 position;		//Light position   X,Y,Z, directional=0 OR positional=1
	vec4 ambient;		//Light ambient color
	vec4 diffuse;		//Light diffuse color
	vec4 specular;		//Light specular color
	vec4 spot_dir;		//Specifies light direction
	int spot_exp;		//Specifies intensity distribution of spotlight
	int spot_cut;		//Specifies maximum spread angle of spotlight (180 = off).
};

struct MaterialInfo {
	vec4 ambient;			//Material ambient reflectivity
	vec4 diffuse;			//Material diffuse reflectivity
	vec4 specular;			//Material specular reflectivity
	float transparency;		//Material transparency factor
	float shininess;		//Material shininess
	int illumination;
};

layout(location = 0) out vec4 FragColor;

//uniform LightInfo light;

uniform sampler2D TexSampler;

in vec4 ecPosition;			
in float ecPartLife;

void main()
{
	if (ecPartLife > 0.0)
		FragColor = texture(TexSampler, gl_PointCoord) * vec4 (0.5, 0, 0, 1);
	else
		FragColor = vec4 (0.0);
}
