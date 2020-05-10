#version 450 core


// has to have same name as vertex shader
in vec2 uv_frag;

// our texture
layout (binding=0) uniform sampler2D tex;

// actual output
// gl_FragColor is deprecated
out vec4 frag_color;

void main(){
    frag_color = texture(tex, uv_frag);
}