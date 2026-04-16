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

    void generate();
};