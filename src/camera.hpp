#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(float worldSize, float heightScale, float aspect);

    void update(float worldSize, float heightScale);
    void setAspect(float aspect);

    glm::mat4 getView() const;
    glm::mat4 getProj() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    float     m_far;
    float m_aspect;
};