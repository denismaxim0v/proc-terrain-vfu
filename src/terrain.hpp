#pragma once
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "camera.hpp"

class Terrain {
public:
    Terrain(int size,
        float tileScale = 1.0f,
        float heightScale = 4.0f,
        int seed = 1337,
        float baseFrequency = 220.0f,
        float gain = 0.5,
        float lacunarity = 2.0f);

    void draw(const Camera& camera);

    float getWorldSize() const { return (float)(m_size) * m_tileScale; }
    float getHeightScale() const { return m_heightScale; }

    float m_heightScale;

private:
    int m_size;
    float m_tileScale;
    int m_seed;
    float m_baseFrequency;
    float m_gain;
    float m_lacunarity;
    int m_indexCount;

    float m_minHeight, m_maxHeight;

    float m_falloffEdge0 = 0.4f;
    float m_falloffEdge1 = 1.0f;

    GLuint m_VBO, m_EBO, m_VAO;
    Shader m_shader;

    GLuint m_texWater;
    GLuint m_texGrass;
    GLuint m_texSoil;
    GLuint m_texSnow;
    GLuint m_texSand;

    void generate();
    GLuint LoadTexture(const std::string& path);
};