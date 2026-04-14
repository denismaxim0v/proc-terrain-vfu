#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 MVP;
uniform float heightScale;

void main()
{
    float h = sin(aPos.x * 5.0) * cos(aPos.z * 5.0);

    vec3 pos = aPos;
    pos.y += h * heightScale;

    gl_Position = MVP * vec4(pos, 1.0);
}