#include <cassert>
#include <cstdint>
#include <exception>
#include <sstream>
#include <string>
#include <tuple>

#include <division_engine_core/types/color.h>

#include <division_engine_core/renderer.h>
#include <division_engine_core/render_pass_instance.h>
#include <division_engine_core/shader.h>
#include <division_engine_core/texture.h>
#include <division_engine_core/uniform_buffer.h>
#include <division_engine_core/vertex_buffer.h>
#include <glm/vec4.hpp>

#include "core/context.hpp"
#include "core/exception.hpp"
#include "utility/file.hpp"

namespace division_engine::core
{

Context::Context(DivisionContext* context)
  : _ctx(context)
{
}

DivisionId
Context::create_bundled_shader(const std::filesystem::path& path_without_extension)
{
    using path = std::filesystem::path;

    auto [vertex_entry_point, vertex_path, fragment_entry_point, fragment_path] =
#if __APPLE__
        std::make_tuple(
            "vert",
            path { path_without_extension }.concat(".vert.metal"),
            "frag",
            path { path_without_extension }.concat(".frag.metal")
        );
#else
        std::make_tuple(
            "main",
            path { path_without_extension }.concat(".vert"),
            "main",
            path { path_without_extension }.concat(".frag")
        );
#endif

    auto vertex_source = division_engine::utility::file::read_text(vertex_path);
    auto fragment_source = division_engine::utility::file::read_text(fragment_path);

    auto shader_descs = std::array {
        DivisionShaderSourceDescriptor {
            .type = DivisionShaderType::DIVISION_SHADER_VERTEX,
            .entry_point_name = vertex_entry_point,
            .source = vertex_source.c_str(),
            .source_size = static_cast<uint32_t>(vertex_source.length()),
        },
        DivisionShaderSourceDescriptor {
            .type = DivisionShaderType::DIVISION_SHADER_FRAGMENT,
            .entry_point_name = fragment_entry_point,
            .source = fragment_source.c_str(),
            .source_size = static_cast<uint32_t>(fragment_source.length()),
        }
    };

    DivisionId shader_program = 0;
    if (!division_engine_shader_program_alloc(
            _ctx, shader_descs.data(), shader_descs.size(), &shader_program
        ))
    {
        throw Exception { std::string { "Failed to create a shader" } };
    }

    return shader_program;
}

void Context::delete_shader(DivisionId shader_id)
{
    division_engine_shader_program_free(_ctx, shader_id);
}

DivisionId Context::create_vertex_buffer(
    std::span<const DivisionVertexAttributeSettings> per_vertex_attributes,
    std::span<const DivisionVertexAttributeSettings> per_instance_attributes,
    VertexBufferSize buffer_size,
    Topology topology
)
{
    const DivisionVertexBufferConstSettings settings {
        .size = buffer_size,
        .per_vertex_attributes = per_vertex_attributes.data(),
        .per_instance_attributes = per_instance_attributes.data(),
        .per_vertex_attribute_count = static_cast<int32_t>(per_vertex_attributes.size()),
        .per_instance_attribute_count =
            static_cast<int32_t>(per_instance_attributes.size()),
        .topology = topology
    };

    DivisionId vertex_buffer_id = 0;
    if (!division_engine_vertex_buffer_alloc(_ctx, &settings, &vertex_buffer_id))
    {
        throw Exception { "Failed to create vertex buffer" };
    }

    return vertex_buffer_id;
}

void Context::resize_vertex_buffer(
    DivisionId vertex_buffer_id,
    DivisionVertexBufferSize new_size
)
{
    if (!division_engine_vertex_buffer_resize(_ctx, vertex_buffer_id, new_size))
    {
        throw Exception { "Failed to resize vertex buffer" };
    }
}

void Context::delete_vertex_buffer(DivisionId vertex_buffer_id)
{
    division_engine_vertex_buffer_free(_ctx, vertex_buffer_id);
}

void Context::delete_uniform(DivisionId buffer_id)
{
    division_engine_uniform_buffer_free(_ctx, buffer_id);
}

DivisionId Context::create_uniform(DivisionUniformBufferDescriptor descriptor)
{
    DivisionId buffer_id = 0;
    if (!division_engine_uniform_buffer_alloc(_ctx, descriptor, &buffer_id))
    {
        throw Exception { "Failed to create uniform buffer" };
    }

    return buffer_id;
}
void Context::draw_render_passes(
    std::span<const DivisionRenderPassInstance> render_pass_instances,
    glm::vec4 clear_color
)
{
    division_engine_render_pass_instance_draw(
        _ctx,
        reinterpret_cast<DivisionColor*>(&clear_color), // NOLINT
        render_pass_instances.data(),
        render_pass_instances.size()
    );
}

DivisionId Context::create_texture(glm::vec2 size, DivisionTextureFormat format)
{
    DivisionTexture texture {
        .channels_swizzle =
            DivisionTextureChannelsSwizzle {
                .red = DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_RED,
                .green = DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_GREEN,
                .blue = DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_BLUE,
                .alpha = DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ALPHA },
        .texture_format = format,
        .min_filter =
            DivisionTextureMinMagFilter::DIVISION_TEXTURE_MIN_MAG_FILTER_NEAREST,
        .mag_filter =
            DivisionTextureMinMagFilter::DIVISION_TEXTURE_MIN_MAG_FILTER_NEAREST,
        .width = static_cast<uint32_t>(size.x),
        .height = static_cast<uint32_t>(size.y),
        .has_channels_swizzle = false,
    };
    DivisionId id = 0;
    if (!division_engine_texture_alloc(_ctx, &texture, &id))
    {
        throw Exception { "Failed to create a texture" };
    }
    return id;
}

void Context::set_texture_data(DivisionId texture_id, const uint8_t* data)
{
    division_engine_texture_set_data(_ctx, texture_id, data);
}

void Context::delete_texture(DivisionId texture_id)
{
    division_engine_texture_free(_ctx, texture_id);
}
glm::vec2 Context::get_screen_size() const
{
    return {
        _ctx->renderer_context->frame_buffer_width,
        _ctx->renderer_context->frame_buffer_height,
    };
}
}