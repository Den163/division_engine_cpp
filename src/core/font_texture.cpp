#include "core/font_texture.hpp"

#include <cstdlib>
#include <cstring>
#include <iterator>
#include <utility>

namespace division_engine::core
{
FontTexture::FontTexture(
    Context& context,
    const std::filesystem::path& font_path,
    size_t font_size,
    glm::ivec2 resolution
)
  : _character_to_glyph_index()
  , _glyphs()
  , _glyph_positions()
  , _free_rows_space()
  , _ctx(context)
  , _resolution(resolution)
  , _font_size(font_size)
  , _rasterizer_buffer_capacity(0)
  , _pixel_buffer(static_cast<uint8_t*>(std::malloc(resolution.x * resolution.y)))
  , _rasterizer_buffer(nullptr)
  , _font_id(_ctx.create_font(font_path, static_cast<uint32_t>(font_size)))
  , _texture_id(context.create_texture(
        resolution,
        DivisionTextureFormat::DIVISION_TEXTURE_FORMAT_R8Uint
    ))
  , _texture_was_changed(false)
{
    std::memset(_pixel_buffer, 0, resolution.x * resolution.y);

    auto row_count = resolution.y / font_size;
    _free_rows_space.resize(
        row_count,
        FreeBlock { .position = 0, .width = static_cast<uint32_t>(resolution.x) }
    );

    auto approx_char_count = row_count + (resolution.x / font_size);
    _glyphs.reserve(approx_char_count);
    _glyph_positions.reserve(approx_char_count);
    _character_to_glyph_index.reserve(approx_char_count);
}

FontTexture::~FontTexture() 
{
    _ctx.delete_texture(_texture_id);
    _ctx.delete_font(_font_id);

    std::free(_pixel_buffer);
    std::free(_rasterizer_buffer);
}

void FontTexture::upload_texture()
{
    if (!_texture_was_changed)
    {
        return;
    }

    _ctx.set_texture_data(_texture_id, _pixel_buffer);
    _texture_was_changed = false;
}

size_t FontTexture::reserve_character(char32_t character)
{
    auto it = _character_to_glyph_index.find(character);
    if (it != _character_to_glyph_index.end())
    {
        return it->second;
    }

    size_t new_index = _character_to_glyph_index.size();

    layout_glyph(character, new_index);
    rasterize_glyph(character, new_index);

    _character_to_glyph_index.emplace(std::make_pair(character, new_index));
    return new_index;
}

void FontTexture::layout_glyph(char32_t character, size_t index)
{
    const auto insert_index = static_cast<decltype(_glyphs)::difference_type>(index);

    const int GLYPH_GAP = 1;

    DivisionFontGlyph glyph = _ctx.get_font_glyph(_font_id, character);
    if (character == U' ')
    {
        glyph.width = glyph.advance_x;
    }

    const auto gapped_glyph_width = static_cast<int>(glyph.width) + GLYPH_GAP;
    for (int row = 0; row < _free_rows_space.size(); row++)
    {
        auto& free_block = _free_rows_space[row];
        const auto free_after = static_cast<int>(free_block.width) - gapped_glyph_width;
        
        if (free_after < 0)
        {
            continue;
        }

        const glm::ivec2 position { free_block.position, _font_size + row };

        free_block.position += gapped_glyph_width;
        free_block.width = free_after;

        _glyphs.insert(_glyphs.begin() + insert_index, glyph);
        _glyph_positions.insert(_glyph_positions.begin() + insert_index, position);

        return;
    }

    throw FontTextureOutOfFreeSpaceException {};
}

void FontTexture::rasterize_glyph(char32_t character, size_t index) 
{
    const auto& glyph = _glyphs[index];
    const auto& position = _glyph_positions[index];

    const auto glyph_bytes = glyph.width * glyph.height;
    if (_rasterizer_buffer_capacity < glyph_bytes)
    {
        _rasterizer_buffer = static_cast<uint8_t*>(
            std::realloc(_rasterizer_buffer, glyph_bytes) // NOLINT
        );
        _rasterizer_buffer_capacity = glyph_bytes;
    }

    if (character != U' ')
    {
        _ctx.rasterize_glyph(_font_id, character, _rasterizer_buffer);
    }
    else
    {
        std::memset(_rasterizer_buffer, 0, _rasterizer_buffer_capacity);
    }

    for (int h = 0; h < glyph.height; h++)
    {
        auto src_row_start = glyph.width * h;
        auto dst_row_start = position.x + (position.y + h) * _resolution.x;

        const auto* src_ptr = _rasterizer_buffer + src_row_start;
        auto* dst_ptr = _pixel_buffer + dst_row_start;
        std::memcpy(dst_ptr, src_ptr, glyph.width);
    }

    _texture_was_changed = true;
}
}