#version 400 core //compatibility
layout(location = 0) out vec4 FragColor;
in vec3 tePosition;

uniform LightInfo light;
uniform sampler2D TexSampler;

//out vec4 out_Color0;
//out vec4 out_Color1;
/*vec4 atmosphere(vec3 pos) 
{
    vec4 c0 = vec4(0.172, 0.290, 0.486, 1.000)*1.25;
    vec4 c1 = vec4(0.321, 0.482, 0.607, 1.000)*1.25;
    vec4 s0 = vec4(5.0, 5.0, 5.0, 1.0) * 0.2; //sun color
    float d = length(pos - lightPos)*100.0;
    if(pos.y >= 0.0)
		return mix(mix(c1,c0,pos.y), s0, clamp(1.0/pow(d,1.1), 0.0, 1.0));
    else
		return mix(c1, s0, clamp(1.0/pow(d,1.1), 0.0, 1.0));
}*/

void main() {
    out_Color0 = atmosphere(tePosition / 5000.0);
    out_Color0.w = 1.0;
    out_Color1 = vec4(tePosition.xyz, 0.0);
}