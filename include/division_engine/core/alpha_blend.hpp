#pragma once

#include <division_engine_core/types/render_pass_descriptor.h>
#include <glm/vec4.hpp>

#include <optional>
#include <cassert>

namespace division_engine::core
{
struct AlphaBlend
{
    AlphaBlend(DivisionAlphaBlend src_blend, DivisionAlphaBlend dst_blend)
      : _src_blend(src_blend)
      , _dst_blend(dst_blend)
      , _constant_color(std::nullopt)
    {
        // NOLINTNEXTLINE
        assert(!is_constant_color_blend(src_blend) & !is_constant_color_blend(dst_blend));
    }

    AlphaBlend(
        DivisionAlphaBlend src_blend,
        DivisionAlphaBlend dst_blend,
        glm::vec4 constant_color)
      : _src_blend(src_blend)
      , _dst_blend(dst_blend)
      , _constant_color(constant_color)
    {
        // NOLINTNEXTLINE
        assert(is_constant_color_blend(src_blend) | is_constant_color_blend(dst_blend));
    }

    static bool is_constant_color_blend(DivisionAlphaBlend alpha_blend)
    {
        return (alpha_blend == DIVISION_ALPHA_BLEND_CONSTANT_ALPHA) |
               (alpha_blend == DIVISION_ALPHA_BLEND_CONSTANT_COLOR) |
               (alpha_blend == DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_COLOR) |
               (alpha_blend == DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_ALPHA);
    }

    bool with_constant_color() const { return _constant_color.has_value(); }

    DivisionAlphaBlend src_blend() const { return _src_blend; }
    DivisionAlphaBlend dst_blend() const { return _dst_blend; }
    glm::vec4 constant_color() const { return _constant_color.value(); }

private:
    DivisionAlphaBlend _src_blend;
    DivisionAlphaBlend _dst_blend;
    std::optional<glm::vec4> _constant_color;
};
}