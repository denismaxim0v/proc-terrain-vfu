#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : Camera(10.0f, 4.0f)
{
}

Camera::Camera(float worldSize, float heightScale)
{
    float zPos = worldSize * 1.2f;
    float yPos = worldSize * 0.8f + heightScale * 0.5f;

    m_position = glm::vec3(0.0f, yPos, zPos);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_far = worldSize * 10.0f;
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(m_position, m_target, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProj(float aspect) const
{
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, m_far);
}