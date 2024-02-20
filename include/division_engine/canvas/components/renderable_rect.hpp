#pragma once

#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace division_engine::canvas::components
{

struct RenderableRect
{
    glm::vec4 color = color::WHITE;
    BorderRadius border_radius = BorderRadius::none();
};

}