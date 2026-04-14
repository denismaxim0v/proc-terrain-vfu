#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use();

    void setFloat(const std::string& name, float v);
    void setMat4(const std::string& name, const float* mat);

    GLuint getID() const { return ID; }

private:
    GLuint ID = 0;

    std::unordered_map<std::string, GLint> uniformCache;

    bool loadFile(const char* path, std::string& out);

    GLuint compile(GLenum type, const std::string& src);
    GLuint linkProgram(GLuint vs, GLuint fs);

    GLint getUniformLocation(const std::string& name);
};