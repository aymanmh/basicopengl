#version 450 core

layout(location = 0) in vec3 vertex;
layout(location = 2) in vec2 uv;

// will be used in fragment shader
out vec2 TexCoords;

void main(){
    TexCoords = uv;
    gl_Position = vec4(vertex, 1.0);
}