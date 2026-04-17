#include "shader.hpp"
#include "logger.hpp"

#include <fstream>
#include <sstream>

bool Shader::loadFile(const char* path, std::string& out)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        Log(LogLevel::Error, "Failed to open shader file: " + std::string(path));
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    out = ss.str();
    return true;
}

GLuint Shader::compile(GLenum type, const std::string& src)
{
    GLuint shader = glCreateShader(type);

    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        std::string log(len, ' ');
        glGetShaderInfoLog(shader, len, nullptr, log.data());

        Log(LogLevel::Error, "Shader compile failed:\n" + log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Shader::linkProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        std::string log(len, ' ');
        glGetProgramInfoLog(program, len, nullptr, log.data());

        Log(LogLevel::Error, "Program link failed:\n" + log);

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vs, fs;

    if (!loadFile(vertexPath, vs) || !loadFile(fragmentPath, fs))
    {
        Log(LogLevel::Error, "Shader file loading failed.");
        ID = 0;
        return;
    }

#ifndef NDEBUG
    Log(LogLevel::Info, "Vertex Shader Loaded:\n" + vs);
    Log(LogLevel::Info, "Fragment Shader Loaded:\n" + fs);
#endif

    GLuint v = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);

    if (v == 0 || f == 0)
    {
        ID = 0;
        return;
    }

    ID = linkProgram(v, f);

    glDeleteShader(v);
    glDeleteShader(f);

    if (ID == 0)
    {
        Log(LogLevel::Error, "Failed to create shader program.");
    }
}

Shader::~Shader()
{
    if (ID != 0)
        glDeleteProgram(ID);
}


void Shader::use()
{
    glUseProgram(ID);
}


GLint Shader::getUniformLocation(const std::string& name)
{
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
        return it->second;

    GLint loc = glGetUniformLocation(ID, name.c_str());
    uniformCache[name] = loc;

    return loc;
}

void Shader::setFloat(const std::string& name, float v)
{
    glUniform1f(getUniformLocation(name), v);
}

void Shader::setMat4(const std::string& name, const float* mat)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat);
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}