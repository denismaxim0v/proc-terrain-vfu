#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(float worldSize, float heightScale, float aspect);

    void update(float worldSize, float heightScale);
    void setAspect(float aspect);

    void orbit(float deltaYaw, float deltaPitch);
    void zoom(float delta);

    glm::mat4 getView() const;
    glm::mat4 getProj() const;

private:
    glm::vec3 m_target;
    float     m_yaw;
    float     m_pitch;
    float     m_distance;
    float     m_far;
    float     m_aspect;

    glm::vec3 position() const;
};