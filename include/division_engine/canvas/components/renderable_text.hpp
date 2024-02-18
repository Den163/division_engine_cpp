#pragma once

#include <glm/vec4.hpp>
#include <string>

namespace division_engine::canvas::components
{

struct RenderableText
{
    glm::vec4 color;
    std::u16string text;
    float font_size;
};

}