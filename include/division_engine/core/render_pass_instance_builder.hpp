#pragma once

#include <division_engine_core/context.h>
#include <division_engine_core/render_pass_instance.h>
#include <span>

#include "types.hpp"

namespace division_engine::core
{
class RenderPassInstanceBuilder
{
public:
    RenderPassInstanceBuilder(DivisionId render_pass_descriptor_id);

    RenderPassInstanceBuilder instances(
        size_t instance_count = 0,
        size_t first_instance = 0);

    RenderPassInstanceBuilder vertices(size_t vertex_count, size_t first_vertex = 0);

    RenderPassInstanceBuilder indices(size_t index_count);

    RenderPassInstanceBuilder uniform_vertex_buffers(
        std::span<DivisionIdWithBinding> vertex_uniforms);

    RenderPassInstanceBuilder uniform_fragment_buffers(
        std::span<DivisionIdWithBinding> fragment_uniforms);

    RenderPassInstanceBuilder fragment_textures(
        std::span<DivisionIdWithBinding> fragment_textures);

    DivisionRenderPassInstance build() const { return _pass; }

private:
    DivisionRenderPassInstance _pass;
};
}