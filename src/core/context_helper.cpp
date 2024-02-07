#include "division_engine_core/vertex_buffer.h"
#include <division_engine/core/context_helper.hpp>

#include <division_engine/core/exception.hpp>
#include <division_engine/utility/file.hpp>

#include <division_engine_core/shader.h>
#include <exception>
#include <sstream>
#include <string>
#include <tuple>

namespace division_engine::core
{

ContextHelper::ContextHelper(DivisionContext* context)
  : _ctx(context)
{
}

DivisionId ContextHelper::create_bundled_shader(
    const std::filesystem::path& path_without_extension)
{
    using path = std::filesystem::path;

    auto [vertex_entry_point, vertex_path, fragment_entry_point, fragment_path] =
#if __APPLE__
        std::make_tuple(
            "vert",
            path { path_without_extension }.concat(".vert.metal"),
            "frag",
            path { path_without_extension }.concat(".frag.metal"));
#else
        std::make_tuple(
            "main",
            path { path_without_extension }.concat(".vert"),
            "main",
            path { path_without_extension }.concat(".frag"));
#endif

    auto vertex_source = division_engine::utility::file::read_text(vertex_path);
    auto fragment_source = division_engine::utility::file::read_text(fragment_path);

    DivisionShaderSourceDescriptor shader_descs[] = {
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

    DivisionId shader_program;
    bool success =
        division_engine_shader_program_alloc(_ctx, shader_descs, 0, &shader_program);
    if (!success)
    {
        throw Exception { std::string { "Failed to create a shader" } };
    }

    return shader_program;
}

void ContextHelper::delete_shader(DivisionId shader_id)
{
    division_engine_shader_program_free(_ctx, shader_id);
}

DivisionId ContextHelper::create_vertex_buffer(
    std::span<const DivisionVertexAttributeSettings> per_vertex_attributes,
    std::span<const DivisionVertexAttributeSettings> per_instance_attributes,
    VertexBufferSize buffer_size,
    Topology topology)
{
    const DivisionVertexBufferSettings settings {
        .size = buffer_size,
        .per_vertex_attributes =
            const_cast<DivisionVertexAttributeSettings*>(per_vertex_attributes.data()),
        .per_instance_attributes =
            const_cast<DivisionVertexAttributeSettings*>(per_instance_attributes.data()),
        .per_vertex_attribute_count = static_cast<int32_t>(per_vertex_attributes.size()),
        .per_instance_attribute_count =
            static_cast<int32_t>(per_instance_attributes.size()),
        .topology = topology
    };

    DivisionId vertex_buffer_id;
    if (!division_engine_vertex_buffer_alloc(_ctx, &settings, &vertex_buffer_id))
    {
        throw Exception { "Failed to create vertex buffer" };
    }

    return vertex_buffer_id;
}
}