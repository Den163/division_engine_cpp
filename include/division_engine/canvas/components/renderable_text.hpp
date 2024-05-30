#pragma once

#include "division_engine/color.hpp"
#include <glm/vec4.hpp>
#include <string>

namespace division_engine::canvas::components
{

struct RenderableText
{
    constexpr static float DEFAULT_FONT_SIZE = 16;

    std::string text;
    glm::vec4 color = color::BLACK;
    float font_size = DEFAULT_FONT_SIZE;
};

}