#pragma once

#include <glm/glm.hpp>


namespace moonrock {

    template <typename T>
    glm::tvec2<T> rotate_vec2_cw_90(glm::tvec2<T> v) {
        return glm::tvec2<T>{ v.y, -v.x };
    }

    template <typename T>
    glm::tvec2<T> rotate_vec2_ccw_90(glm::tvec2<T> v) {
        return glm::tvec2<T>{ -v.y, v.x };
    }

}
