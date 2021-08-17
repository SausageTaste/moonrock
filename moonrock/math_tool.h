#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace moonrock {

    template <typename T>
    glm::tvec2<T> rotate_vec2_cw_90(glm::tvec2<T> v) {
        return glm::tvec2<T>{ v.y, -v.x };
    }

    template <typename T>
    glm::tvec2<T> rotate_vec2_ccw_90(glm::tvec2<T> v) {
        return glm::tvec2<T>{ -v.y, v.x };
    }

    template <typename T>
    auto interpolate_barycentric(const T& v0, const T& v1, const T& v2, const glm::vec3& barycentric) {
        return v0 * barycentric[0] + v1 * barycentric[1] + v2 * barycentric[2];
    }

}
