#include "terrain.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "perlin_noise.hpp"
#include <iostream>

#include "camera.hpp"
#include "logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Vertex {
    float x, y, z;     // position
    float nx, ny, nz;  // normal
    float u, v;        // UV
};

static float GetHeight(
    PerlinNoise<float>& noise,
    float x,
    float z,
    float scale = 220.0f,
    float offset = 0.0f,
    float octaves = 6,
    float gain = 0.5f,
    float lacunarity = 2.0f
)
{
    float sum = 0.0f;
    float max = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < octaves; i++) {
        float noiseVal = noise.Noise(
            (x + offset) / scale * frequency,
            0.0f,
            (z + offset) / scale * frequency
        );
        sum += noiseVal * amplitude;
        max += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return (sum / max + 1.0f) / 2.0f;
}

Terrain::Terrain(int size, float tileScale, float heightScale,
    int seed, float baseFrequency, float gain, float lacunarity)
    : m_shader("./shaders/terrain.vert", "./shaders/terrain.frag")
{
    m_size = size;
    m_tileScale = tileScale;
    m_heightScale = heightScale;
    m_seed = seed;
    m_baseFrequency = baseFrequency;
    m_gain = gain;
    m_lacunarity = lacunarity;

    m_texSand = LoadTexture("./assets/sand.jpg");
    m_texGrass = LoadTexture("./assets/grass.jpg");
    m_texSoil = LoadTexture("./assets/soil.jpg");
    m_texSnow = LoadTexture("./assets/snow.jpg");

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    PerlinNoise<float> noise(m_seed);

    float worldSize = getWorldSize();

    int vertCount = (m_size + 1) * (m_size + 1);
    std::vector<float> heights(vertCount);
    float minH = FLT_MAX;
    float maxH = -FLT_MAX;

    for (int z = 0; z <= m_size; z++) {
        for (int x = 0; x <= m_size; x++) {
            float h = GetHeight(noise, (float)x, (float)z,
                m_baseFrequency, 0.0f, 6, m_gain, m_lacunarity);
            heights[z * (m_size + 1) + x] = h;
            minH = std::min(minH, h);
            maxH = std::max(maxH, h);
        }
    }

    m_minHeight = minH * m_heightScale - m_heightScale * 0.5f;
    m_maxHeight = maxH * m_heightScale - m_heightScale * 0.5f;

    for (int z = 0; z <= m_size; z++) {
        for (int x = 0; x <= m_size; x++) {
            float xf = (float)x / m_size;
            float zf = (float)z / m_size;

            float normalizedHeight = heights[z * (m_size + 1) + x];

            float fx = glm::abs(xf * 2.0f - 1.0f);
            float fz = glm::abs(zf * 2.0f - 1.0f);
            float falloff = glm::smoothstep(m_falloffEdge0, m_falloffEdge1,
                glm::max(fx, fz));

            float finalHeight = glm::clamp(normalizedHeight - falloff, 0.0f, 1.0f);

            Vertex v;
            v.x = xf * worldSize - worldSize * 0.5f;
            v.z = zf * worldSize - worldSize * 0.5f;
            v.y = finalHeight * m_heightScale - m_heightScale * 0.5f;
            v.u = xf;
            v.v = zf;
            v.nx = v.ny = v.nz = 0.0f;

            vertices.push_back(v);
        }
    }

    float minFinal = FLT_MAX;
    float maxFinal = -FLT_MAX;
    for (auto& v : vertices) {
        minFinal = std::min(minFinal, v.y);
        maxFinal = std::max(maxFinal, v.y);
    }
    m_minHeight = minFinal;
    m_maxHeight = maxFinal;

    Log(LogLevel::Info, m_minHeight, ":", m_maxHeight);

    for (int z = 0; z < m_size; z++) {
        for (int x = 0; x < m_size; x++) {

            int i = z * (m_size + 1) + x;

            indices.push_back(i);
            indices.push_back(i + m_size + 1);
            indices.push_back(i + 1);

            indices.push_back(i + 1);
            indices.push_back(i + m_size + 1);
            indices.push_back(i + m_size + 2);
        }
    }

    m_indexCount = (int)indices.size();

    for (auto& v : vertices) {
        v.nx = v.ny = v.nz = 0.0f;
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 p0(v0.x, v0.y, v0.z);
        glm::vec3 p1(v1.x, v1.y, v1.z);
        glm::vec3 p2(v2.x, v2.y, v2.z);

        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;

        glm::vec3 normal = glm::cross(edge1, edge2);

        v0.nx += normal.x; v0.ny += normal.y; v0.nz += normal.z;
        v1.nx += normal.x; v1.ny += normal.y; v1.nz += normal.z;
        v2.nx += normal.x; v2.ny += normal.y; v2.nz += normal.z;
    }

    for (auto& v : vertices) {
        glm::vec3 n(v.nx, v.ny, v.nz);
        n = glm::normalize(n);

        v.nx = n.x;
        v.ny = n.y;
        v.nz = n.z;
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

GLuint Terrain::LoadTexture(const std::string& path)
{
    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
    if (!data)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return tex;
}

void Terrain::draw(const Camera& camera)
{
    m_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texWater);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texSand);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texGrass);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_texSoil);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_texSnow);

    m_shader.setInt("texWater", 0);
    m_shader.setInt("texSand", 1);
    m_shader.setInt("texGrass", 2);
    m_shader.setInt("texSoil", 3);
    m_shader.setInt("texSnow", 4);

    glm::mat4 MVP =
        camera.getProj() *
        camera.getView() *
        glm::mat4(1.0f);

    m_shader.setMat4("MVP", glm::value_ptr(MVP));
    m_shader.setFloat("texTiling", getWorldSize() * 1.5f);
    m_shader.setFloat("minHeight", m_minHeight);
    m_shader.setFloat("maxHeight", m_maxHeight);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}