#pragma once

#include "division_engine/canvas/border_radius.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace division_engine::canvas::components
{

struct RenderableRect
{
    glm::vec4 color;
    BorderRadius border_radius;
};

}