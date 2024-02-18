#pragma once

#include "state.hpp"

namespace division_engine::canvas
{
template<typename T>
concept Renderer = requires(T renderer, State& state) {
                       {
                           renderer.update(state)
                       };
                   };
}