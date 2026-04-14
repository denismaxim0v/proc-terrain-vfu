#pragma once
#include <vector>
#include <glad/glad.h>

class Terrain {
public:
    Terrain(int size);
    void draw();

    GLuint VAO;
    int indexCount;

private:
    GLuint VBO, EBO;
    int size;

    void generate();
};