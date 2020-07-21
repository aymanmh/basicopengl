#version 450 core

uniform int Pass;  // Pass number
uniform float Weight[5];
uniform float LumThresh;
uniform bool bloom;
uniform float exposure;
uniform float gamma;

//uniform vec4 vertColor;
in vec2 TexCoords;
in vec4 color;
in float brightnessFactor;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bright_color;
layout (binding=0) uniform sampler2D myTexture1;
layout (binding=1) uniform sampler2D sceneTexture;


void renderToHdr(out vec4 birghtnessColor, out vec4 fragColor)
{

//birghtnessColor = vec4(0.0, 0.0, 0.0, 1.0);
//fragColor = vec4(0.5,0.5,0.0,1.0);
//return;
    vec4 result = texture(myTexture1, gl_PointCoord) * color;
    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > LumThresh)
        birghtnessColor = result * brightnessFactor;
        //birghtnessColor = vec4(1.0,0.0,0.0,result.a);
    else
        birghtnessColor = vec4(0.0, 0.0, 0.0, result.a);
    //birghtnessColor = vec4(1.0,0.0,0.0,result.a);
    fragColor = result;
}

vec4 pass2() {
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(myTexture1, pix, 0) * Weight[0];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,1) ) * Weight[1];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,-1) ) * Weight[1];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,2) ) * Weight[2];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,-2) ) * Weight[2];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,3) ) * Weight[3];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,-3) ) * Weight[3];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,4) ) * Weight[4];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(0,-4) ) * Weight[4];
    return  sum;
    //return vec4(1.0,0.0,0.0,1.0);
}

vec4 pass3() {
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(myTexture1, pix, 0) * Weight[0];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(1,0) ) * Weight[1];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(-1,0) ) * Weight[1];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(2,0) ) * Weight[2];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(-2,0) ) * Weight[2];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(3,0) ) * Weight[3];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(-3,0) ) * Weight[3];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(4,0) ) * Weight[4];
    sum += texelFetchOffset( myTexture1, pix, 0, ivec2(-4,0) ) * Weight[4];
    return sum;
}

vec4 pass4()
{
    float alpha = texture(sceneTexture, TexCoords).a;
    vec3 hdrColor = texture(sceneTexture, TexCoords).rgb;      
    vec3 bloomColor = texture(myTexture1, TexCoords).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * 1.0);
    // gamma correction
    result = pow(result, vec3(1.0 / gamma));
    return vec4(result, alpha);
}

void main(void)
{

if (Pass == 0)
    frag_color =  texture(myTexture1, gl_PointCoord) * color;
else if(Pass == 1)
    renderToHdr(bright_color,frag_color);
else if (Pass == 2)
    frag_color = pass2();
else if (Pass == 3)
    frag_color = pass3();
else if (Pass == 4)
    frag_color =  pass4();
else if (Pass == 5)
    frag_color = texture(myTexture1,TexCoords);
}