#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    glm::mat4 matrix;

    void UpdateMatrix() {
        matrix = glm::translate(position) * glm::toMat4(orientation) * glm::scale(scale);
    }
};
