#ifndef CAMERA_H
#define CAMERA_H

#include <glm/mat4x4.hpp>

class Camera {
public:
    Camera();

    const glm::mat4& get_projection() const;
    const glm::mat4& get_view() const;
private:
    glm::mat4 projection;
    glm::mat4 view;
};

#endif // !CAMERA_H
