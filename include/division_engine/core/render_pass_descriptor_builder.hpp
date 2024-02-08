#pragma once

#include <division_engine_core/context.h>
#include <division_engine_core/render_pass_descriptor.h>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>
#include <cassert>
#include <cstring>
#include <optional>

#include "alpha_blend.hpp"
#include "exception.hpp"
#include "types.hpp"

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
        DivisionAlphaBlendOperation alpha_blend_op);

    DivisionId build();

private:
    DivisionRenderPassDescriptor _desc;
    DivisionContext* _ctx;
    bool _init_shader;
    bool _init_vertex_buffer;
};
}