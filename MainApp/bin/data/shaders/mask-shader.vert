#version 150
#pragma include "utils.glsl"

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;


uniform vec4 globalColor;
uniform vec2 screen;
uniform vec2 mouse;
uniform float time;

// set automatically by OF
in vec4 position;
in vec3 normal;
in vec4 color;
in vec2 texcoord;

out vec3 vPos;
out vec4 vColor;
out vec2 vTex;
out float vDepth;

void main() {    
    vColor = globalColor;
    vTex = texcoord;
    vPos = position.xyz;
    gl_Position = modelViewProjectionMatrix * position;
}