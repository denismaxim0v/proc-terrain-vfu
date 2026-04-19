#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;


uniform mat4 MVP;

out vec2 vUV;
out vec3 vNormal;
out vec3 vPos;

void main()
{
    vUV    = aUV;
    vNormal = aNormal;
    vPos   = aPos;   // NEW: world position passed through
    gl_Position = MVP * vec4(aPos, 1.0);
}