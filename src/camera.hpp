#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(float worldSize, float heightScale);

    glm::mat4 getView() const;
    glm::mat4 getProj(float aspect) const;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    float     m_far;
};