#pragma once

#include "division_engine/core/exception.hpp"
#include "division_engine_core/vertex_buffer.h"
#include <division_engine/core/types.hpp>
#include <division_engine/core/vertex_data.hpp>

#include <division_engine_core/context.h>

#include <filesystem>

namespace division_engine::core
{
using VertexBufferSize = DivisionVertexBufferSize;
using Topology = DivisionRenderTopology;

class ContextHelper
{
public:
    ContextHelper(DivisionContext* context);

    DivisionId create_bundled_shader(const std::filesystem::path& path_without_extension);
    void delete_shader(DivisionId shader_id);

    template<VertexData TVertexData, VertexData TInstanceData>
    DivisionId create_vertex_buffer(VertexBufferSize buffer_size, Topology topology)
    {
        return create_vertex_buffer(
            TVertexData::vertex_attributes,
            TInstanceData::vertex_attributes,
            buffer_size,
            topology);
    }

    DivisionId create_vertex_buffer(
        std::span<const DivisionVertexAttributeSettings> per_vertex_attributes,
        std::span<const DivisionVertexAttributeSettings> per_instance_attributes,
        VertexBufferSize buffer_size,
        Topology topology);

    void delete_vertex_buffer(DivisionId vertex_buffer_id);

private:
    DivisionContext* _ctx;
};
}