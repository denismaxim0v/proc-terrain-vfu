#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

static constexpr float kPitchMin = 0.05f;
static constexpr float kPitchMax = 1.55f;

static float initialDistance(float worldSize, float heightScale)
{
    float z = worldSize * 1.2f;
    float y = worldSize * 0.8f + heightScale * 0.5f;
    return std::sqrt(z * z + y * y);
}

static float initialPitch(float worldSize, float heightScale)
{
    float z = worldSize * 1.2f;
    float y = worldSize * 0.8f + heightScale * 0.5f;
    return std::atan2(y, z);
}

Camera::Camera()
    : Camera(10.0f, 4.0f, 1.0f)
{
}

Camera::Camera(float worldSize, float heightScale, float aspect)
    : m_target(0.0f)
    , m_yaw(0.0f)
    , m_pitch(initialPitch(worldSize, heightScale))
    , m_distance(initialDistance(worldSize, heightScale))
    , m_far(worldSize * 10.0f)
    , m_aspect(aspect)
{
}

void Camera::update(float worldSize, float heightScale)
{
    m_distance = initialDistance(worldSize, heightScale);
    m_far = worldSize * 10.0f;
}

void Camera::setAspect(float aspect)
{
    m_aspect = aspect;
}

void Camera::orbit(float deltaYaw, float deltaPitch)
{
    m_yaw += deltaYaw;
    m_pitch = glm::clamp(m_pitch + deltaPitch, kPitchMin, kPitchMax);
}

void Camera::zoom(float delta)
{
    m_distance = glm::max(0.5f, m_distance * (1.0f + delta));
}

glm::vec3 Camera::position() const
{
    return m_target + glm::vec3(
        m_distance * glm::cos(m_pitch) * glm::sin(m_yaw),
        m_distance * glm::sin(m_pitch),
        m_distance * glm::cos(m_pitch) * glm::cos(m_yaw)
    );
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(position(), m_target, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProj() const
{
    return glm::perspective(glm::radians(45.0f), m_aspect, 0.1f, m_far);
}
