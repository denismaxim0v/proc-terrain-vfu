#version 330 core

in vec2  vUV;
in float vHeight;
in vec3  vNormal;

out vec4 FragColor;

uniform sampler2D texSand;
uniform sampler2D texGrass;
uniform sampler2D texSoil;
uniform sampler2D texSnow;

uniform float texTiling;

vec3 blendTerrain(float h, vec2 uv)
{
    float sandW  = 1.0 - smoothstep(0.10, 0.30, h);
    
    float grassW = smoothstep(0.10, 0.25, h) * (1.0 - smoothstep(0.35, 0.50, h));
    
    float soilW  = smoothstep(0.30, 0.45, h) * (1.0 - smoothstep(0.55, 0.70, h));
    
    float snowW  = smoothstep(0.45, 0.70, h);

    vec4 w = vec4(sandW, grassW, soilW, snowW);
    w /= max(dot(w, vec4(1.0)), 0.0001);

    vec3 sand  = texture(texSand,  uv * texTiling).rgb;
    vec3 grass = texture(texGrass, uv * texTiling).rgb;
    vec3 soil  = texture(texSoil,  uv * texTiling).rgb;
    vec3 snow  = texture(texSnow,  uv * texTiling).rgb;

    return sand * w.x + grass * w.y + soil * w.z + snow * w.w;
}

vec3 applyLighting(vec3 color, vec3 normal)
{
    vec3  lightDir = normalize(vec3(0.4, 1.0, 0.3));
    float diffuse  = max(dot(normalize(normal), lightDir), 0.0);
    float ambient  = 0.25;
    return color * (ambient + diffuse * 0.75);
}

void main()
{
    vec3 color = blendTerrain(vHeight, vUV);
    color = applyLighting(color, vNormal);
    FragColor = vec4(color, 1.0);
}