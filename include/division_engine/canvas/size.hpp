#pragma once

#include "glm/ext/vector_float2.hpp"
#include <glm/vec2.hpp>
#include <limits>

namespace division_engine::canvas
{

struct Size
{
    float width;
    float height;

    Size(const glm::vec2& size_vec)
      : width(size_vec.x)
      , height(size_vec.y)
    {
    }

    operator glm::vec2() const { return glm::vec2 { width, height }; }

    static Size unconstrainted()
    {
        return Size { glm::vec2 { std::numeric_limits<float>::infinity() } };
    }

    bool width_unconstrainted() const
    {
        return width == std::numeric_limits<float>::infinity();
    }

    bool height_unconstrainted() const
    {
        return height == std::numeric_limits<float>::infinity();
    }

    bool is_unconstrainted() const
    {
        return width_unconstrainted() & height_unconstrainted();
    }
};
}