#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 MVP;
uniform float heightScale;

out vec2 vUV;
out float vHeight;

void main()
{
    float h = sin(aPos.x * 5.0) * cos(aPos.z * 5.0);

    vec3 pos = aPos;
    pos.y += h * heightScale;

    vUV = aUV;
    vHeight = pos.y;

    gl_Position = MVP * vec4(pos, 1.0);
}