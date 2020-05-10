#version 450 core

uniform vec4 vertColor;
in vec4 color;
layout(location = 0) out vec4 frag_color;

layout (binding=0) uniform sampler2D myTexture1;
//layout (binding=1) uniform sampler2D myTexture2;

void main(void)
{
	//frag_color = color;
	frag_color = texture(myTexture1, gl_PointCoord) * color;//vec4(0.0,0.22,0.0,1.0);
	//frag_color = mix(texture(myTexture1,TextCoord),texture(myTexture2,TextCoord),0.8);
}