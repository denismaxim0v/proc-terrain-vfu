#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    position = glm::vec3(0, 1.5f, 2.5f);
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(
        position,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
}

glm::mat4 Camera::getProj(float aspect) const
{
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}