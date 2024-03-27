#pragma once

#include "state.hpp"

#include <type_traits>

namespace division_engine::canvas
{
class Renderer // NOLINT
{
public:
    virtual void fill_render_queue(State& state) = 0;
    virtual ~Renderer() = default;
};
}