#pragma once

#include "alpha_blend.hpp"
#include "exception.hpp"

#include <division_engine_core/types/render_pass_descriptor.h>
#include <division_engine_core/types/id.h>

#include <glm/vec4.hpp>

#include <cassert>
#include <cstring>
#include <optional>

struct DivisionContext;

namespace division_engine::core
{
class RenderPassDescriptorBuilder
{
public:
    RenderPassDescriptorBuilder(DivisionContext* context_ptr);

    RenderPassDescriptorBuilder shader(DivisionId shader_id);
    RenderPassDescriptorBuilder vertex_buffer(DivisionId vertex_buffer_id);

    RenderPassDescriptorBuilder enable_aplha_blending(
        AlphaBlend alpha_blend,
        DivisionAlphaBlendOperation alpha_blend_op
    );

    DivisionId build();

private:
    DivisionRenderPassDescriptor _desc;
    DivisionContext* _ctx;
    bool _init_shader;
    bool _init_vertex_buffer;
};
}