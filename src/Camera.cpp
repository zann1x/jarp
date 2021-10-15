#include "Camera.h"

#include <glm/ext.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera()
    : projection{ glm::perspective(glm::radians(45.0f), (float)1600 / (float)900, 0.1f, 100.0f) },
    view{ glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 3.0f) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) }
{
}

const glm::mat4& Camera::get_projection() const
{
    return this->projection;
}

const glm::mat4& Camera::get_view() const
{
    return this->view;
}
