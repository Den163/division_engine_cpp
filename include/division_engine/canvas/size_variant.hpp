#pragma once

#include "glm/ext/vector_float2.hpp"
#include <glm/vec2.hpp>
#include <variant>

namespace division_engine::canvas
{

struct SizeVariant
{
    struct Filled { };

    struct Fixed
    {
        glm::vec2 size;
    };

    std::variant<Filled, Fixed> variant;

    static SizeVariant fill() { return SizeVariant { Filled {} }; }

    static SizeVariant fixed(const glm::vec2& size)
    {
        return SizeVariant { Fixed { size } };
    }

    bool is_fixed() const { return std::holds_alternative<Fixed>(variant); }
    bool is_filled() const { return std::holds_alternative<Filled>(variant); }
};
}