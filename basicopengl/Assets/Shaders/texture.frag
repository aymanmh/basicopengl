#version 450 core


uniform int Pass;  // Pass number
uniform float Weight[5];
// has to have same name as vertex shader
in vec2 TexCoords;

// our texture
layout (binding=0) uniform sampler2D hdrTexture;

// actual output
// gl_FragColor is deprecated
out vec4 frag_color;

vec4 pass2() {
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(hdrTexture, pix, 0) * Weight[0];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,1) ) * Weight[1];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,-1) ) * Weight[1];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,2) ) * Weight[2];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,-2) ) * Weight[2];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,3) ) * Weight[3];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,-3) ) * Weight[3];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,4) ) * Weight[4];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(0,-4) ) * Weight[4];
    return  sum;
    //return vec4(1.0,0.0,0.0,1.0);
}

vec4 pass3() {
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(hdrTexture, pix, 0) * Weight[0];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(1,0) ) * Weight[1];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(-1,0) ) * Weight[1];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(2,0) ) * Weight[2];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(-2,0) ) * Weight[2];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(3,0) ) * Weight[3];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(-3,0) ) * Weight[3];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(4,0) ) * Weight[4];
    sum += texelFetchOffset( hdrTexture, pix, 0, ivec2(-4,0) ) * Weight[4];
    return sum;
}

void main(){

    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrTexture, TexCoords).rgb;
  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    frag_color = vec4(mapped, 1.0);
    //frag_color = pass2();
    //frag_color = texture(tex, uv_frag);
}