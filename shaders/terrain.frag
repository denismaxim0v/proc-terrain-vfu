#version 330 core

in vec2 vUV;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

uniform sampler2D texWater;
uniform sampler2D texSand;
uniform sampler2D texGrass;
uniform sampler2D texSoil;
uniform sampler2D texSnow;

uniform float texTiling;
uniform float minHeight;
uniform float maxHeight;

float inverseLerp(float a, float b, float x)
{
    return clamp((x - a) / (b - a), 0.0, 1.0);
}

vec3 blendTerrain(float h, vec2 uv)
{
    vec3 waterTex     = texture(texWater, uv * texTiling).rgb;
    vec3 waterDeepCol    = mix(waterTex, vec3(0.05, 0.15, 0.55), 0.75);
    vec3 waterShallowCol = mix(waterTex, vec3(0.21, 0.40, 0.78), 0.8);

    float waterDeepW    = 1.0 - smoothstep(0.0,  0.10, h);
    float waterShallowW = smoothstep(0.00, 0.10, h)
                        * (1.0 - smoothstep(0.10, 0.30, h));
    float sandW         = smoothstep(0.10, 0.30, h)
                        * (1.0 - smoothstep(0.22, 0.32, h));
    float grassW        = smoothstep(0.15, 0.20, h)
                        * (1.0 - smoothstep(0.55, 0.75, h));
    float soilW         = smoothstep(0.40, 0.55, h)
                        * (1.0 - smoothstep(0.70, 0.79, h));
    float snowW         = smoothstep(0.75, 0.90, h);

    float sum = waterDeepW + waterShallowW + sandW + grassW + soilW + snowW;
    sum = max(sum, 0.0001);
    waterDeepW    /= sum;
    waterShallowW /= sum;
    sandW         /= sum;
    grassW        /= sum;
    soilW         /= sum;
    snowW         /= sum;

    vec3 sand  = texture(texSand,  uv * texTiling).rgb;
    vec3 grass = texture(texGrass, uv * texTiling).rgb;
    vec3 soil  = texture(texSoil,  uv * texTiling).rgb;
    vec3 snow  = texture(texSnow,  uv * texTiling).rgb;

    return waterDeepCol    * waterDeepW
         + waterShallowCol * waterShallowW
         + sand            * sandW
         + grass           * grassW
         + soil            * soilW
         + snow            * snowW;
}

vec3 applyLighting(vec3 color, vec3 normal)
{
    vec3 n = normalize(normal);

    vec3  sunDir   = normalize(vec3(1.0, 0.15, 0.3));
    vec3  sunColor = vec3(1.0, 0.55, 0.22);
    float diffuse  = max(dot(n, sunDir), 0.0);

    vec3  skyColor    = vec3(0.22, 0.32, 0.58);
    vec3 groundColor = vec3(0.4, 0.3, 0.25);
    float skyBlend = clamp(n.y * 0.5 + 0.5, 0.6, 1.0);
    vec3  ambient     = mix(groundColor, skyColor, skyBlend) * 0.6 + vec3(0.25);

    return color * (ambient + sunColor * diffuse * 0.95);
}

void main()
{
    float h = inverseLerp(minHeight, maxHeight, vPos.y);

    vec3 color = blendTerrain(h, vUV);
    color = applyLighting(color, vNormal);
    FragColor = vec4(color, 1.0);
}