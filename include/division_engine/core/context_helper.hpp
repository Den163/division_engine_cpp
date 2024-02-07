#pragma once

#include "division_engine/core/exception.hpp"
#include "division_engine_core/vertex_buffer.h"
#include <_types/_uint32_t.h>
#include <division_engine/core/types.hpp>
#include <division_engine/core/vertex_data.hpp>

#include <division_engine_core/context.h>

#include <filesystem>
#include <sys/_types/_int32_t.h>

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

    DivisionId create_vertex_buffer(
        std::span<const DivisionVertexAttributeSettings> per_vertex_attributes,
        std::span<const DivisionVertexAttributeSettings> per_instance_attributes,
        VertexBufferSize buffer_size,
        Topology topology);

    template<typename TVertexData, typename TInstanceData>
    DivisionId create_vertex_buffer(VertexBufferSize buffer_size, Topology topology)
    {
        create_vertex_buffer(
            TVertexData::vertex_attributes,
            TInstanceData::vertex_attributes,
            buffer_size,
            topology);
    }

private:
    DivisionContext* _ctx;
};
}