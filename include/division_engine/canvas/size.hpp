#pragma once

#include "glm/ext/vector_float2.hpp"
#include <cmath>
#include <glm/vec2.hpp>
#include <limits>

namespace division_engine::canvas
{

struct Size
{
    static constexpr float UNCONSTRAINTED = std::numeric_limits<float>::infinity();

    float width;
    float height;

    Size(float width, float height)
      : width(width)
      , height(height) {};

    Size(const glm::vec2& size_vec)
      : Size(size_vec.x, size_vec.y)
    {
    }

    operator glm::vec2() const { return glm::vec2 { width, height }; }

    static Size unconstrainted() { return Size { UNCONSTRAINTED, UNCONSTRAINTED }; }

    static Size with_unconstrainted_width(float height)
    {
        return Size { UNCONSTRAINTED, height };
    }

    static Size with_unconstrainted_height(float width)
    {
        return Size { width, UNCONSTRAINTED };
    }

    bool width_unconstrainted() const { return std::isinf(width); }

    bool height_unconstrainted() const { return std::isinf(height); }

    bool is_unconstrainted() const
    {
        return width_unconstrainted() & height_unconstrainted();
    }
};
}