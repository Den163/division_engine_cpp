#pragma once

#include "glm/ext/vector_float2.hpp"
#include <glm/vec2.hpp>

namespace division_engine::canvas
{
struct Rect
{
    glm::vec2 center {};
    // Half size
    glm::vec2 extents {};

    static Rect from_center(glm::vec2 center, glm::vec2 size)
    {
        return Rect { center, size * 0.5f }; // NOLINT
    }

    static Rect from_top_right(glm::vec2 top_right, glm::vec2 size)
    {
        auto extents = size * 0.5f; // NOLINT
        return Rect { top_right - extents, extents };
    }

    static Rect from_top_left(glm::vec2 top_left, glm::vec2 size)
    {
        auto extents = size * 0.5f; // NOLINT
        return Rect { { top_left.x + extents.x, top_left.y - extents.y }, extents };
    }

    static Rect from_bottom_right(glm::vec2 bottom_right, glm::vec2 size)
    {
        auto extents = size * 0.5f; // NOLINT
        return Rect { { bottom_right.x - extents.x, bottom_right.y + extents.y } };
    }

    static Rect from_bottom_left(glm::vec2 bottom_left, glm::vec2 size)
    {
        auto extents = size * 0.5f; // NOLINT
        return Rect { bottom_left + extents, extents };
    }

    glm::vec2 size() const { return extents * 2.f; /* NOLINT */ }
    float area() const { return extents.x * 2.f * extents.y * 2.f; /* NOLINT */ }

    float left() const { return center.x - extents.x; }
    float right() const { return center.x + extents.x; }
    float top() const { return center.y + extents.y; }
    float bottom() const { return center.y - extents.y; }

    void set_left(float value) { center.x = value + extents.x; }
    void set_right(float value) { center.x = value - extents.x; }
    void set_top(float value) { center.y = value - extents.y; }
    void set_bottom(float value) { center.y = value + extents.y; }

    bool contains(Rect rect) const
    {
        return (left() <= rect.left()) & (rect.right() <= right()) &
               (bottom() <= rect.bottom()) & (rect.top() <= top());
    }

    bool contains(glm::vec2 point) const
    {
        return (left() <= point.x) & (point.x <= right()) & (bottom() <= point.y) &
               (point.y <= top());
    }
};
}