#pragma once
#include <string>
#include <glad/glad.h>
#include "shader.hpp"
#include "camera.hpp"

class Skybox {
public:
    Skybox(const std::string& directory);
    ~Skybox();

    void draw(const Camera& camera);

private:
    GLuint m_VAO, m_VBO;
    GLuint m_cubemapTexture;
    Shader m_shader;

    GLuint loadCubemap(const std::string& directory);
};
