#include "core/render_pass_instance_builder.hpp"

#include <division_engine_core/render_pass_instance.h>

namespace division_engine::core
{
RenderPassInstanceBuilder::RenderPassInstanceBuilder(DivisionId render_pass_descriptor_id)
  : _pass(DivisionRenderPassInstance {
        .vertex_count = 0,
        .index_count = 0,
        .instance_count = 0,
        .uniform_vertex_buffer_count = 0,
        .uniform_fragment_buffer_count = 0,
        .fragment_texture_count = 0,
        .render_pass_descriptor_id = render_pass_descriptor_id,
        .capabilities_mask = DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_NONE,
    })
{
}

RenderPassInstanceBuilder
RenderPassInstanceBuilder::instances(size_t instance_count, size_t first_instance)
{
    _pass.capabilities_mask = static_cast<DivisionRenderPassInstanceCapabilityMask>(
        _pass.capabilities_mask |
        DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_INSTANCED_RENDERING
    );

    _pass.instance_count = instance_count;
    _pass.first_instance = first_instance;

    return *this;
}

RenderPassInstanceBuilder
RenderPassInstanceBuilder::vertices(size_t vertex_count, size_t first_vertex)
{
    _pass.vertex_count = vertex_count;
    _pass.first_vertex = first_vertex;

    return *this;
}

RenderPassInstanceBuilder RenderPassInstanceBuilder::indices(size_t index_count)
{
    _pass.index_count = index_count;

    return *this;
}

RenderPassInstanceBuilder RenderPassInstanceBuilder::uniform_vertex_buffers(
    std::span<DivisionIdWithBinding> vertex_uniforms
)
{
    _pass.uniform_vertex_buffers = vertex_uniforms.data(),
    _pass.uniform_vertex_buffer_count = static_cast<int32_t>(vertex_uniforms.size());

    return *this;
}

RenderPassInstanceBuilder RenderPassInstanceBuilder::uniform_fragment_buffers(
    std::span<DivisionIdWithBinding> fragment_uniforms
)
{
    _pass.uniform_fragment_buffers = fragment_uniforms.data();
    _pass.uniform_fragment_buffer_count = static_cast<int32_t>(fragment_uniforms.size());

    return *this;
}

RenderPassInstanceBuilder RenderPassInstanceBuilder::fragment_textures(
    std::span<DivisionIdWithBinding> fragment_textures
)
{
    _pass.fragment_textures = fragment_textures.data();
    _pass.fragment_texture_count = static_cast<int32_t>(fragment_textures.size());

    return *this;
}
}
