#include "division_engine/canvas/text_drawer.hpp"

#include "core/render_pass_instance_builder.hpp"
#include "division_engine/core/alpha_blend.hpp"
#include "flecs/addons/cpp/mixins/query/builder.hpp"
#include "glm/ext/vector_float2.hpp"

#include <division_engine_core/types/id.h>
#include <division_engine_core/types/render_pass_descriptor.h>
#include <division_engine_core/types/vertex_buffer.h>
#include <flecs.h>

#include <filesystem>
#include <ranges>
#include <string>

namespace division_engine::canvas
{

const auto RASTERIZED_FONT_SIZE = 64.f;
const auto INSTANCE_CAPACITY = 1024;
const auto SCREEN_SIZE_UNIFORM_LOCATION = 1;
const auto TEXTURE_LOCATION = 0;

TextDrawer::TextDrawer(
    Context& context,
    State& state,
    const std::filesystem::path& font_path
)
  : _font_texture(
        FontTexture { context, font_path, static_cast<size_t>(RASTERIZED_FONT_SIZE) }
    )
  , _ctx(context)
  , _instance_capacity(INSTANCE_CAPACITY)
  , _screen_size_uniform(DivisionIdWithBinding {
        .id = state.screen_size_uniform_id,
        .shader_location = SCREEN_SIZE_UNIFORM_LOCATION,
    })
  , _shader_id(_ctx.create_bundled_shader(
        std::filesystem::path { "resources" } / "shaders" / "canvas" / "font"
    ))
  , _vertex_buffer_id(_ctx.create_vertex_buffer<TextCharVertex, TextCharInstance>(
        DivisionVertexBufferSize {
            .instance_count = INSTANCE_CAPACITY,
            .vertex_count = RECT_VERTICES.size(),
            .index_count = RECT_INDICES.size(),
        },
        DIVISION_TOPOLOGY_TRIANGLES
    ))
  , _render_pass_descriptor_id(
        _ctx.render_pass_descriptor_builder()
            .shader(_shader_id)
            .vertex_buffer(_vertex_buffer_id)
            .enable_aplha_blending(
                core::AlphaBlend {
                    DIVISION_ALPHA_BLEND_SRC_ALPHA,
                    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA,
                },
                DIVISION_ALPHA_BLEND_OP_ADD
            )
            .build()
    )
{
    auto vb_data =
        _ctx.borrow_vertex_buffer_data<TextCharVertex, TextCharInstance>(_vertex_buffer_id
        );

    std::ranges::copy(RECT_VERTICES, vb_data.per_vertex_data().data());
    std::ranges::copy(RECT_INDICES, vb_data.index_data().data());

    _texture_bindings.push_back(DivisionIdWithBinding {
        .id = _font_texture.texture_id(),
        .shader_location = TEXTURE_LOCATION,
    });

    _query = state.world.query_builder<RenderBounds, RenderableText, RenderOrder>()
                 .order_by<RenderOrder>([](auto, const auto* x, auto, const auto* y)
                                        { return x->compare(*y); })
                 .build();
}

TextDrawer::~TextDrawer()
{
    _ctx.delete_vertex_buffer(_vertex_buffer_id);
    _ctx.delete_shader(_shader_id);
}

void TextDrawer::update(State& state)
{
    using core::RenderPassInstanceBuilder;

    size_t overall_instance_count = 0;

    auto vb_data =
        _ctx.borrow_vertex_buffer_data<TextCharVertex, TextCharInstance>(_vertex_buffer_id
        );

    auto instances = vb_data.per_instance_data();

    _query.each(
        [&](RenderBounds& bounds, RenderableText& renderable, RenderOrder& render_order)
        {
            const auto& text_str = renderable.text;
            for (auto ch : text_str)
            {
                _font_texture.reserve_character(ch);
            }

            const auto needed_capacity = overall_instance_count + text_str.size();
            if (_instance_capacity < needed_capacity)
            {
                _ctx.resize_vertex_buffer(
                    _vertex_buffer_id,
                    DivisionVertexBufferSize {
                        .vertex_count = RECT_VERTICES.size(),
                        .index_count = RECT_INDICES.size(),
                        .instance_count = static_cast<uint32_t>(needed_capacity) }
                );
                _instance_capacity = needed_capacity;
            }

            const auto font_scale = renderable.font_size / RASTERIZED_FONT_SIZE;
            const auto space_index = _font_texture.reserve_character(u' ');
            const auto space_glyph = _font_texture.glyph_at(space_index);
            const auto space_advance_x =
                static_cast<float>(space_glyph.advance_x) * font_scale;

            const auto bounds_rect = bounds.value;
            const auto font_size = renderable.font_size;

            glm::vec2 pen_pos { bounds_rect.left(), bounds_rect.top() - font_size };
            size_t rendered_char_count = 0;

            size_t i = 0;
            while (i < text_str.size())
            {
                const auto ch = text_str[i];

                switch (ch)
                {
                    case u' ':
                    {
                        pen_pos.x += space_advance_x;
                        i += 1;
                        continue;
                    }
                    case u'\n':
                    {
                        pen_pos = { bounds_rect.left(), pen_pos.y - font_size };
                        i += 1;
                        continue;
                    }
                }

                if (pen_pos.y < bounds_rect.bottom())
                {
                    break;
                }

                using str_diff_type = std::u16string::difference_type;

                const auto word_start_it =
                    text_str.begin() + static_cast<str_diff_type>(i);
                const auto word = get_next_word(
                    std::u16string_view {
                        word_start_it,
                        text_str.end(),
                    },
                    font_scale
                );

                if (pen_pos.x + word.width > bounds_rect.right())
                {
                    pen_pos = { bounds_rect.left(), pen_pos.y - font_size };
                }

                if ((word.width > bounds_rect.size().x) |
                    (pen_pos.y < bounds_rect.bottom()))
                {
                    break;
                }

                const auto word_instance_index =
                    overall_instance_count + rendered_char_count;

                const auto word_end_it =
                    text_str.begin() +
                    static_cast<str_diff_type>(i + word.character_count);
                add_word_to_vertex_buffer(
                    { word_start_it, word_end_it },
                    pen_pos,
                    renderable.color,
                    font_scale,
                    instances.subspan(word_instance_index, word.character_count)
                );

                pen_pos.x += word.width;
                rendered_char_count += word.character_count;
                i += word.character_count;
            }

            overall_instance_count += rendered_char_count;
        }
    );

    const auto pass =
        RenderPassInstanceBuilder { _render_pass_descriptor_id }
            .instances(overall_instance_count)
            .vertices(RECT_VERTICES.size())
            .indices(RECT_INDICES.size())
            .fragment_textures({ &_texture_bindings[0], 1 })
            .uniform_fragment_buffers({ &_screen_size_uniform, 1 })
            .uniform_vertex_buffers({ &_screen_size_uniform, 1 })
            .build();

    state.render_queue.enqueue_pass(pass, 1);
    _font_texture.upload_texture();
}

TextDrawer::WordInfo
TextDrawer::get_next_word(const std::u16string_view& text, float font_scale) const
{
    WordInfo word { 0, 0 };

    for (int i = 0; i < text.size(); i++)
    {
        const auto ch = text[i];
        if (ch == u' ' | ch == u'\n')
        {
            break;
        }

        const auto glyph_index = _font_texture.glyph_index(ch);
        const auto& glyph = _font_texture.glyph_at(glyph_index);

        word.width += static_cast<float>(glyph.advance_x) * font_scale;
        word.character_count += 1;
    }

    return word;
}

void TextDrawer::add_word_to_vertex_buffer(
    const std::u16string_view& word,
    const glm::vec2& position,
    const glm::vec4& color,
    float font_scale,
    std::span<TextCharInstance> instances
)
{
    auto word_pos = position;
    for (int i = 0; i < word.size(); i++)
    {
        const auto ch = word[i];
        const auto char_idx = _font_texture.glyph_index(ch);
        const auto& glyph = _font_texture.glyph_at(char_idx);

        if (glyph.width <= 0)
        {
            continue;
        }

        const auto& glyph_pos = _font_texture.glyph_position_at(char_idx);
        const auto scaled_advance = static_cast<float>(glyph.advance_x) * font_scale;
        const auto glyph_size = glm::vec2 { glyph.width, glyph.height };
        const auto scaled_size = glyph_size * font_scale;
        const auto offset = glm::vec2 {
            glyph.left,
            static_cast<float>(glyph.top) - static_cast<float>(glyph.height),
        } * font_scale;

        const auto instance_position = word_pos + offset;
        instances[i] = TextCharInstance {
            .texel_coord = glyph_pos,
            .size = scaled_size,
            .position = instance_position,
            .color = color,
            .glyph_in_tex_size = glyph_size,
            .tex_size = _font_texture.texture_size()
        };

        word_pos.x += scaled_advance;
    }
}
}