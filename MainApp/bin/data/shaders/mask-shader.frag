#version 150
#pragma include "utils.glsl"

out vec4 fragColor;
uniform sampler2D tex0;
uniform sampler2D mask;

in vec4 vColor;
in vec3 vPos;
in vec2 vTex;

void main() {
    
    vec2 tc = vec2(vTex.x, vTex.y);
    vec2 ftc = vec2(tc.x, tc.y);
    vec3 color = vColor.xyz;
    
    vec4 imageColor = texture(tex0, ftc);
    vec4 maskColor = texture(mask, ftc);
    float a = maskColor.r;

    if (imageColor.r < 0.01 && imageColor.b < 0.01) discard;

    fragColor = vec4(imageColor.xyz, a);
    //fragColor = maskColor;
    //fragColor = imageColor;
}