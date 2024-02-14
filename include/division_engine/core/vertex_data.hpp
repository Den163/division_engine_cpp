#pragma once

#include "exception.hpp"
#include "types.hpp"

#include <division_engine_core/shader.h>
#include <division_engine_core/vertex_buffer.h>
#include <glm/fwd.hpp>

#include <concepts>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#define DIVISION_IMPL_VERTEX_ATTR_SPEC(c_type, division_type)                            \
    template<>                                                                           \
    constexpr DivisionVertexAttributeSettings make_vertex_attribute<c_type>(int location \
    )                                                                                    \
    {                                                                                    \
        return DivisionVertexAttributeSettings {                                         \
            .type = division_type,                                                       \
            .location = location,                                                        \
        };                                                                               \
    }

namespace division_engine::core
{
template<typename T>
concept VertexData = std::constructible_from<decltype(T::vertex_attributes)>;

template<typename T>
constexpr DivisionVertexAttributeSettings make_vertex_attribute(int location);

DIVISION_IMPL_VERTEX_ATTR_SPEC(int, DivisionShaderVariableType::DIVISION_INTEGER)
DIVISION_IMPL_VERTEX_ATTR_SPEC(float, DivisionShaderVariableType::DIVISION_FLOAT)
DIVISION_IMPL_VERTEX_ATTR_SPEC(double, DivisionShaderVariableType::DIVISION_DOUBLE)
DIVISION_IMPL_VERTEX_ATTR_SPEC(glm::vec2, DivisionShaderVariableType::DIVISION_FVEC2)
DIVISION_IMPL_VERTEX_ATTR_SPEC(glm::vec3, DivisionShaderVariableType::DIVISION_FVEC3)
DIVISION_IMPL_VERTEX_ATTR_SPEC(glm::vec4, DivisionShaderVariableType::DIVISION_FVEC4)
DIVISION_IMPL_VERTEX_ATTR_SPEC(glm::mat4x4, DivisionShaderVariableType::DIVISION_FMAT4X4)
};