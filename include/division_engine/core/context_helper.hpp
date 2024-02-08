#pragma once

#include "division_engine/core/exception.hpp"
#include "division_engine/core/render_pass.hpp"
#include <division_engine/core/types.hpp>
#include <division_engine/core/uniform_data.hpp>
#include <division_engine/core/vertex_data.hpp>

#include <division_engine_core/context.h>

#include <filesystem>
#include <span>

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

    template<typename TVertexData, typename TInstanceData>
    VertexBufferData<TVertexData, TInstanceData> get_vertex_buffer_data(
        DivisionId vertex_buffer_id)
    {
        return VertexBufferData<TVertexData, TInstanceData> { _ctx, vertex_buffer_id };
    }

    void delete_vertex_buffer(DivisionId vertex_buffer_id);

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

    void delete_uniform(DivisionId buffer_id);

    RenderPassDescriptorBuilder render_pass_descriptor_builder()
    {
        return RenderPassDescriptorBuilder { _ctx };
    }

private:
    DivisionContext* _ctx;

    DivisionId create_uniform(DivisionUniformBufferDescriptor descriptor);
};
}