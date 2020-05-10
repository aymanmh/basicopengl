#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 in_color;

uniform mat4 mvp;

out vec4 color;
void main(void)
{
    //gl_Position = vec4(pos.x +posOffset.x,pos.y+posOffset.y,pos.z,1.0);
    //gl_PointSize = 100.0;
    gl_Position = mvp * vec4(pos,1.0);	
    //gl_Position =  vec4(pos,1.0);
	color = vec4(in_color,1.0);
    //float brightness = (color.r * 0.2126) + (color.g * 0.7152) + (color.b * 0.0722); 
}