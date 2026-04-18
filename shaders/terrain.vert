#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in float aHeight;

uniform mat4 MVP;
uniform float heightScale;

out vec2 vUV;
out float vHeight;
out vec3  vNormal;

void main()
{
    vUV     = aUV;
    vHeight = aHeight;
    vNormal = aNormal;
    gl_Position = MVP * vec4(aPos, 1.0);
}