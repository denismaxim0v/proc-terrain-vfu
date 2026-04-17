#pragma once
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "camera.hpp"

class Terrain {
public:
    Terrain(int size);
    void draw(Camera camera);

    GLuint m_VAO;
    int m_indexCount;
    Shader m_shader;
    float m_heightScale;

private:
    GLuint m_VBO, m_EBO;
    int m_size;
    GLuint m_texGrass;
    GLuint m_texSoil;
    GLuint m_texSnow;
    GLuint m_texSand;

    void generate();
    GLuint LoadTexture(const std::string& path);
};