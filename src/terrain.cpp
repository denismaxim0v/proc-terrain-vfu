#include "terrain.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "perlin_noise.hpp"

#include "camera.hpp"

struct Vertex {
    float x, y, z;     // position
    float nx, ny, nz;  // normal
    float u, v;        // UV
};

static float GetHeight(PerlinNoise<float>& noise, float x, float z)
{
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;

    int octaves = 4;
    float persistence = 0.5f;

    for (int i = 0; i < octaves; i++) {
        total += noise.Noise(
            x * frequency,
            0.0f,
            z * frequency
        ) * amplitude;

        frequency *= 2.0f;
        amplitude *= persistence;
    }

    return total;
}

Terrain::Terrain(int size) : m_shader("./shaders/terrain.vert", "./shaders/terrain.frag")
{
    m_size = size;
    m_heightScale = 0.3f;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    PerlinNoise<float> noise(1337);

    for (int z = 0; z <= m_size; z++) {
        for (int x = 0; x <= m_size; x++) {

            float xf = (float)x / m_size;
            float zf = (float)z / m_size;

            float baseFrequency = 3.0f;
            float height = GetHeight(noise, xf * baseFrequency, zf * baseFrequency);

            Vertex v;

            v.x = xf * 2.0f - 1.0f;
            v.y = height * m_heightScale;
            v.z = zf * 2.0f - 1.0f;

            v.u = xf;
            v.v = zf;

            v.nx = 0.0f;
            v.ny = 1.0f;
            v.nz = 0.0f;

            vertices.push_back(v);
        }
    }

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

void Terrain::draw(Camera camera)
{
    m_shader.use();
    glm::mat4 MVP =
        camera.getProj(1280.0f / 720.0f) *
        camera.getView() *
        glm::mat4(1.0f);

    m_shader.setMat4("MVP", glm::value_ptr(MVP));
    m_shader.setFloat("heightScale", m_heightScale);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}