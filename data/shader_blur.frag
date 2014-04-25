#version 400 core

//precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

layout(location = 0) out vec4 FragColor;

uniform sampler2D TexSampler;
uniform sampler2D shadowMap;

uniform vec2 pixelSize;
uniform uint blur;

in vec2 UV;
in vec4 ShadowCoord;

void main()
{ 
	if(blur>0)  {
		vec4 result;
		result+=1.0f*texture(TexSampler,vec2(UV.x-pixelSize.x,UV.y-pixelSize.y));
		result+=2.0f*texture(TexSampler,vec2(UV.x,UV.y-pixelSize.y));
		result+=1.0f*texture(TexSampler,vec2(UV.x+pixelSize.x,UV.y-pixelSize.y));
		result+=2.0f*texture(TexSampler,vec2(UV.x-pixelSize.x,UV.y));
		result+=3.0f*texture(TexSampler,vec2(UV.x,UV.y));
		result+=2.0f*texture(TexSampler,vec2(UV.x+pixelSize.x,UV.y));
		result+=1.0f*texture(TexSampler,vec2(UV.x-pixelSize.x,UV.y+pixelSize.y));
		result+=2.0f*texture(TexSampler,vec2(UV.x,UV.y+pixelSize.y));
		result+=1.0f*texture(TexSampler,vec2(UV.x+pixelSize.x,UV.y+pixelSize.y));
		result = result / 15.0f;
	
		FragColor =  result;
	} else {
		//float visibility = texture(shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w));
		//if (visibility < 0.9)
		//	visibility = 0.5;

		FragColor = vec4(texture (TexSampler, UV).rgb,1.0);// * visibility;
	
	}
}
