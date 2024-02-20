#pragma once

#include "state.hpp"
#include <type_traits>

namespace division_engine::canvas
{
template<typename T>
concept Renderer = requires(T renderer, State& state) {
                       {
                           renderer.update(state)
                       };
                   } && std::is_move_constructible<T>();
}