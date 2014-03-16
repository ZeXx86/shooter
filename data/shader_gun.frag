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

layout(location = 0) out vec4 FragColor;

uniform LightInfo light;
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
  /* vec3 N = normalize(ecNormal);
   vec3 L = normalize(ecLightDir);

   float lambert = dot(N,L);
   
   if (lambert>0.0)   
   {
      FragColor = vec4(1.0,0.0,0.0,1.0);
   }      
   else
   {
      FragColor = vec4(0.0,0.0,0.0,1.0);
   }*/
   
   FragColor = vec4(texture (TexSampler, UV).rgb,1.0);
}
