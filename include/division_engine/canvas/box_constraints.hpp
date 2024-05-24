#pragma once

#include "size.hpp"
#include <glm/vec2.hpp>

namespace division_engine::canvas
{
struct BoxConstraints
{
    glm::vec2 min_size;
    glm::vec2 max_size;
};
}