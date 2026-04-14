#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    glm::mat4 getView() const;
    glm::mat4 getProj(float aspect) const;

private:
    glm::vec3 position;
};