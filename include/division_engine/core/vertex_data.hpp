#pragma once

#include "division_engine_core/vertex_buffer.h"
#include <concepts>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
namespace division_engine::core
{
template<typename T>
concept VertexData = std::constructible_from<decltype(T::vertex_attributes)>;
};