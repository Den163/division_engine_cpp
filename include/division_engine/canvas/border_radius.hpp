#pragma once

#include <glm/vec4.hpp>

namespace division_engine::canvas
{
struct BorderRadius
{
    glm::vec4 top_left_right_bottom;

    static BorderRadius none() { return BorderRadius { glm::vec4 { 0 } }; }

    static BorderRadius all(float value) { return BorderRadius { glm::vec4 { value } }; }

    static BorderRadius symmetric(float horizontal, float vertical)
    {
        return BorderRadius { { vertical, horizontal, horizontal, vertical } };
    }
};
}