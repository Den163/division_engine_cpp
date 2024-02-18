#pragma once

#include "division_engine/canvas/rect.hpp"

namespace division_engine::canvas::components
{
struct RenderBounds
{
    Rect value;

    RenderBounds(RenderBounds&&) = default;
    RenderBounds& operator=(const RenderBounds&) = default;
    RenderBounds& operator=(RenderBounds&&) = default;
    RenderBounds(RenderBounds&) = default;
    ~RenderBounds() = default;

    RenderBounds()
      : value()
    {
    }

    RenderBounds(const Rect& rect)
      : value(rect)
    {
    }
};

}