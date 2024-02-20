#pragma once

#include "division_engine/core/context.hpp"
#include "glm/ext/vector_int2.hpp"

#include <cstddef>
#include <cstdint>
#include <division_engine_core/types/font.h>
#include <division_engine_core/types/id.h>
#include <exception>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

namespace division_engine::core
{
class FontTexture
{
public:
    constexpr static const glm::ivec2 DEFAULT_RESOLUTION { 1024, 512 };

    FontTexture() = delete;
    FontTexture(const FontTexture&) = delete;
    FontTexture& operator=(const FontTexture&) = delete;

    FontTexture& operator=(FontTexture&&) = default;
    FontTexture(FontTexture&& other) noexcept
        : _character_to_glyph_index(std::move(other._character_to_glyph_index))
        , _glyphs(std::move(other._glyphs))
        , _glyph_positions(std::move(other._glyph_positions))
        , _free_rows_space(std::move(other._free_rows_space))
        , _ctx(other._ctx)
        , _resolution(other._resolution)
        , _font_size(other._font_size)
        , _rasterizer_buffer_capacity(other._rasterizer_buffer_capacity)
        , _pixel_buffer(other._pixel_buffer)
        , _rasterizer_buffer(other._rasterizer_buffer)
        , _font_id(other._font_id)
        , _texture_id(other._texture_id)
        , _texture_was_changed(other._texture_was_changed)
    {
        other._pixel_buffer = nullptr;
        other._rasterizer_buffer = nullptr;
    }

    FontTexture(
        Context& context,
        const std::filesystem::path& font_path,
        size_t font_size,
        glm::ivec2 resolution = DEFAULT_RESOLUTION
    );

    ~FontTexture();

    DivisionId texture_id() const { return _texture_id; }
    glm::ivec2 texture_size() const { return _resolution; }

    const DivisionFontGlyph& glyph_at(size_t index) const { return _glyphs[index]; }
    const glm::ivec2& glyph_position_at(size_t index) const
    {
        return _glyph_positions[index];
    }

    void upload_texture();
    size_t reserve_character(char32_t character);
    size_t glyph_index(char32_t character) const
    {
        return _character_to_glyph_index.at(character);
    }

private:
    struct FreeBlock
    {
        uint32_t position;
        uint32_t width;
    };

    std::unordered_map<char32_t, size_t> _character_to_glyph_index;

    std::vector<DivisionFontGlyph> _glyphs;
    std::vector<glm::ivec2> _glyph_positions;

    std::vector<FreeBlock> _free_rows_space;

    Context _ctx;

    glm::ivec2 _resolution;
    size_t _font_size;
    size_t _rasterizer_buffer_capacity;

    uint8_t* _pixel_buffer;
    uint8_t* _rasterizer_buffer;

    DivisionId _font_id;
    DivisionId _texture_id;

    bool _texture_was_changed;

    void layout_glyph(char32_t character, size_t index);
    void rasterize_glyph(char32_t character, size_t index);
};

class FontTextureOutOfFreeSpaceException : std::exception
{
    const char* what() const noexcept override
    {
        return "There is no free space in font texture";
    }
};

}