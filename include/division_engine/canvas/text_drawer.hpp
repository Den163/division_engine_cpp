#pragma once

#include "components/render_bounds.hpp"
#include "components/render_order.hpp"
#include "components/render_texture.hpp"
#include "components/renderable_text.hpp"
#include "state.hpp"

#include "division_engine/core/context.hpp"
#include "division_engine/core/font_texture.hpp"
#include "division_engine/core/vertex_data.hpp"

#include <division_engine_core/types/id.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace division_engine::canvas
{
class TextDrawer
{
public:
    using FontTexture = core::FontTexture;
    using Context = core::Context;
    using RenderableText = components::RenderableText;
    using RenderBounds = components::RenderBounds;
    using RenderOrder = components::RenderOrder;
    using RenderTexture = components::RenderTexture;

    struct TextCharVertex
    {
        glm::vec2 position;
        glm::vec2 uv;

        static constexpr auto vertex_attributes = std::array {
            core::make_vertex_attribute<decltype(position)>(0),
            core::make_vertex_attribute<decltype(uv)>(1),
        };
    } __attribute__((__packed__));

    struct TextCharInstance
    {
        glm::vec4 color;
        glm::vec2 texel_coord;
        glm::vec2 size;
        glm::vec2 position;
        glm::vec2 glyph_in_tex_size;
        glm::vec2 tex_size;

        static constexpr auto vertex_attributes = std::array {
            core::make_vertex_attribute<decltype(color)>(2),
            core::make_vertex_attribute<decltype(texel_coord)>(3),
            core::make_vertex_attribute<decltype(size)>(4),
            core::make_vertex_attribute<decltype(position)>(5),
            core::make_vertex_attribute<decltype(glyph_in_tex_size)>(6),
            core::make_vertex_attribute<decltype(tex_size)>(7),
        };
    } __attribute__((__packed__));

    static constexpr auto RECT_VERTICES = std::array {
        TextCharVertex { .position = glm::vec2 { 0, 1 }, .uv = glm::vec2 { 0, 0 } },
        TextCharVertex { .position = glm::vec2 { 0, 0 }, .uv = glm::vec2 { 0, 1 } },
        TextCharVertex { .position = glm::vec2 { 1, 0 }, .uv = glm::vec2 { 1, 1 } },
        TextCharVertex { .position = glm::vec2 { 1, 1 }, .uv = glm::vec2 { 1, 0 } }
    };

    static constexpr auto RECT_INDICES = std::array { 0, 1, 2, 2, 3, 0 };

    TextDrawer() = delete;
    TextDrawer& operator=(const TextDrawer&) = delete;
    TextDrawer& operator=(TextDrawer&&) = delete;
    TextDrawer(TextDrawer&) = delete;
    TextDrawer(TextDrawer&&) = delete;

    TextDrawer(Context& context, State& state, const std::filesystem::path& font_path);
    ~TextDrawer();

    void update(State& state);

private:
    struct WordInfo
    {
        size_t character_count;
        float width;
    };

    FontTexture _font_texture;
    std::vector<DivisionIdWithBinding> _texture_bindings;
    flecs::query<const RenderBounds, const RenderableText, const RenderOrder> _query;

    Context _ctx;

    size_t _instance_capacity;

    DivisionIdWithBinding _screen_size_uniform;
    DivisionId _shader_id;
    DivisionId _vertex_buffer_id;
    DivisionId _render_pass_descriptor_id;

    WordInfo get_next_word(const std::u16string_view& text, float font_scale) const;

    void add_word_to_vertex_buffer(
        const std::u16string_view& word,
        const glm::vec2& position,
        const glm::vec4& color,
        float font_scale,
        std::span<TextCharInstance> instances
    );
};
}