#pragma once

#include <filesystem>
#include <span>

#include <division_engine_core/types/font.h>
#include <division_engine_core/types/id.h>
#include <division_engine_core/types/render_pass_instance.h>
#include <division_engine_core/types/texture.h>

#include <glm/vec2.hpp>

#include "exception.hpp"
#include "render_pass_descriptor_builder.hpp"
#include "uniform_data.hpp"
#include "vertex_buffer_data.hpp"
#include "vertex_data.hpp"

namespace division_engine::core
{
using VertexBufferSize = DivisionVertexBufferSize;
using Topology = DivisionRenderTopology;

class Context
{
public:
    Context(DivisionContext* context);

    template<VertexData TVertexData, VertexData TInstanceData>
    DivisionId create_vertex_buffer(VertexBufferSize buffer_size, Topology topology)
    {
        return create_vertex_buffer(
            TVertexData::vertex_attributes,
            TInstanceData::vertex_attributes,
            buffer_size,
            topology
        );
    }

    template<typename TVertexData, typename TInstanceData>
    VertexBufferData<TVertexData, TInstanceData>
    borrow_vertex_buffer_data(DivisionId vertex_buffer_id)
    {
        return VertexBufferData<TVertexData, TInstanceData> { _ctx, vertex_buffer_id };
    }

    template<typename T>
    DivisionId create_uniform()
    {
        DivisionUniformBufferDescriptor desc { .data_bytes = sizeof(T) };
        return create_uniform(desc);
    }

    template<typename T>
    UniformData<T> get_uniform_data(DivisionId uniform_id)
    {
        return UniformData<T> { _ctx, uniform_id };
    }

    glm::vec2 get_screen_size() const;

    DivisionContext* get_ptr() { return _ctx; }

    RenderPassDescriptorBuilder render_pass_descriptor_builder()
    {
        return RenderPassDescriptorBuilder { _ctx };
    }

    DivisionId create_bundled_shader(const std::filesystem::path& path_without_extension);
    void delete_shader(DivisionId shader_id);

    DivisionId create_vertex_buffer(
        std::span<const DivisionVertexAttributeSettings> per_vertex_attributes,
        std::span<const DivisionVertexAttributeSettings> per_instance_attributes,
        VertexBufferSize buffer_size,
        Topology topology
    );

    void
    resize_vertex_buffer(DivisionId vertex_buffer_id, DivisionVertexBufferSize new_size);
    void delete_vertex_buffer(DivisionId vertex_buffer_id);

    DivisionId create_uniform(DivisionUniformBufferDescriptor descriptor);
    void delete_uniform(DivisionId buffer_id);

    DivisionId create_texture(glm::ivec2 size, DivisionTextureFormat format);
    void set_texture_data(DivisionId texture_id, const uint8_t* data);
    void delete_texture(DivisionId texture_id);

    void draw_render_passes(
        std::span<const DivisionRenderPassInstance> render_pass_instances,
        glm::vec4 clear_color
    );

    DivisionId create_font(const std::filesystem::path& font_path, uint32_t font_height);
    DivisionFontGlyph get_font_glyph(DivisionId font_id, char32_t character);
    void rasterize_glyph(DivisionId font_id, char32_t character, uint8_t* buffer);
    void delete_font(DivisionId font_id);

private:
    DivisionContext* _ctx;
};
}