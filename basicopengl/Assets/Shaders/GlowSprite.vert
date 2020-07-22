#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 in_color;
layout (location = 4) in vec2 in_uv;
layout (location = 2) in float in_brightness;
layout (location = 3) in float in_size;
uniform mat4 mvp;

out vec2 TexCoords;
out vec4 color;
out float brightnessFactor;

void main(void)
{
    TexCoords = in_uv;

    gl_PointSize = in_size;
    gl_Position = mvp * vec4(pos,1.0);	
    brightnessFactor = in_brightness;
	color = vec4(in_color,1.0);
}