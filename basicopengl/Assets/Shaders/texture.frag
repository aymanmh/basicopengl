#version 450 core


// has to have same name as vertex shader
in vec2 uv_frag;

// our texture
layout (binding=0) uniform sampler2D hdrTexture;

// actual output
// gl_FragColor is deprecated
out vec4 frag_color;

void main(){

    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrTexture, uv_frag).rgb;
  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    frag_color = vec4(mapped, 1.0);
    //frag_color = texture(tex, uv_frag);
}