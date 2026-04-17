#version 330 core

in vec2 vUV;
in float vHeight;

out vec4 FragColor;

uniform sampler2D texSand;
uniform sampler2D texGrass;
uniform sampler2D texSoil;
uniform sampler2D texSnow;

vec3 blendTerrain(float h, vec2 uv)
{
    float sandW  = 1.0 - smoothstep(0.10, 0.35, h);
    float grassW = smoothstep(0.10, 0.35, h) * (1.0 - smoothstep(0.35, 0.55, h));
    float soilW  = smoothstep(0.30, 0.50, h) * (1.0 - smoothstep(0.60, 0.75, h));
    float snowW  = smoothstep(0.50, 0.85, h);

    float sum = sandW + grassW + soilW + snowW;
    vec4 w = vec4(sandW, grassW, soilW, snowW) / max(sum, 0.0001);

    vec3 sand  = texture(texSand,  uv * 10.0).rgb;
    vec3 grass = texture(texGrass, uv * 10.0).rgb;
    vec3 soil  = texture(texSoil,  uv * 10.0).rgb;
    vec3 snow  = texture(texSnow,  uv * 10.0).rgb;

    vec3 color =
        sand  * w.x +
        grass * w.y +
        soil  * w.z +
        snow  * w.w;

    return color;
}

void main()
{
    vec3 color = blendTerrain(vHeight, vUV);
    FragColor = vec4(color, 1.0);
}