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
    vec3 waterDeepCol    = mix(waterTex, vec3(0.00, 0.00, 0.388), 0.8);
    vec3 waterShallowCol = mix(waterTex, vec3(0.21, 0.40, 0.78), 0.8);

    float waterDeepW    = 1.0 - smoothstep(0.0,  0.10, h);
    float waterShallowW = smoothstep(0.00, 0.10, h)
                        * (1.0 - smoothstep(0.10, 0.15, h));
    float sandW         = smoothstep(0.10, 0.15, h)
                        * (1.0 - smoothstep(0.15, 0.20, h));
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
    vec3  lightDir = normalize(vec3(0.4, 1.0, 0.3));
    float diffuse  = max(dot(normalize(normal), lightDir), 0.0);
    float ambient  = 0.25;
    return color * (ambient + diffuse * 0.75);
}

void main()
{
    float h = inverseLerp(minHeight, maxHeight, vPos.y);

    vec3 color = blendTerrain(h, vUV);
    color = applyLighting(color, vNormal);
    FragColor = vec4(color, 1.0);
}