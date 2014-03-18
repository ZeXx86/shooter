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

uniform LightInfo light;
uniform MaterialInfo material;

// Values that stay constant for the whole mesh.
uniform sampler2D TexSampler;



//The prefix ec means Eye Coordinates in the Eye Coordinate System
in vec4 ecPosition;			
in vec3 ecLightDir;
in vec3 ecNormal;
in vec3 ecViewDir;

// Interpolated values from the vertex shaders
in vec2 UV;


void main()
{
   vec4 texColor = vec4(texture (TexSampler, UV).rgb,1.0);
   FragColor = light.ambient * material.ambient * vec4(texture (TexSampler, UV).rgb,1.0);
   
   vec3 N = normalize(ecNormal);
   vec3 L = normalize(ecLightDir);
   
   float lambert = dot(N,L);
   
   if (lambert>0.0)   
   {
		FragColor += light.diffuse * material.diffuse * texColor * lambert;
		vec3 E = normalize(ecViewDir);
		vec3 R = normalize( 2.0 * dot(N, ecLightDir) * N - ecLightDir); 
		float specular = pow(max(dot(R,E), 0.0), 40.0 );
		FragColor += light.specular * material.specular * texColor * specular;
	}
}
