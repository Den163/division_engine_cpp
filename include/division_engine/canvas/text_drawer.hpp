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
    using renderable_type =
        std::tuple<components::RenderableText, components::RenderBounds>;

    struct TextCharVertex
    {
        glm::vec2 position;
        glm::vec2 uv;

        static constexpr auto vertex_attributes =
            std::array { DIVISION_DECLARE_VERTEX_ATTRIBUTE(position, 0),
                         DIVISION_DECLARE_VERTEX_ATTRIBUTE(uv, 1) };
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
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(color, 2),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(texel_coord, 3),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(size, 4),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(position, 5),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(glyph_in_tex_size, 6),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(tex_size, 7),
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
    TextDrawer(TextDrawer&) = delete;
    TextDrawer& operator=(TextDrawer&&) = default;
    
    TextDrawer(TextDrawer&& other) noexcept
      : _font_texture(std::move(other._font_texture))
      , _texture_bindings(std::move(other._texture_bindings))
      , _query(std::move(other._query))
      , _ctx(other._ctx)
      , _instance_capacity(other._instance_capacity)
      , _screen_size_uniform(other._screen_size_uniform)
      , _shader_id(other._shader_id)
      , _vertex_buffer_id(other._vertex_buffer_id)
      , _render_pass_descriptor_id(other._render_pass_descriptor_id)
      , _resources_owner(true)
    {
        other._resources_owner = false;
    };

    TextDrawer(State& state, const std::filesystem::path& font_path);
    ~TextDrawer();

    void update(State& state);

private:
    using Context = core::Context;
    using RenderableText = components::RenderableText;
    using RenderBounds = components::RenderBounds;
    using FontTexture = core::FontTexture;
    using RenderOrder = components::RenderOrder;
    using RenderTexture = components::RenderTexture;

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

    bool _resources_owner;

    WordInfo get_next_word(const std::u16string_view& text, float font_scale) const;

    size_t add_renderable_to_vertex_buffer(
        std::span<TextCharInstance> instances,
        const RenderBounds& bounds,
        const RenderableText& renderable
    );

    void add_word_to_vertex_buffer(
        const std::u16string_view& word,
        const glm::vec2& position,
        const glm::vec4& color,
        float font_scale,
        std::span<TextCharInstance> instances
    );
};
}