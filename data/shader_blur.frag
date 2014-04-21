#version 400 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D TexSampler;
uniform vec2 pixelSize;

in vec2 UV;

void main()
{
   //vec2 pixelSize = vec2(0.00078125,0.00125);
   
   
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
	//FragColor =  vec4(texture (TexSampler, UV).rgb,1.0);
	FragColor =  result;
}
