#include "division_engine/core/alpha_blend.hpp"
#include "division_engine/core/exception.hpp"
#include "division_engine/core/types.hpp"
#include "division_engine_core/context.h"
#include "division_engine_core/render_pass_descriptor.h"
#include "glm/fwd.hpp"
#include "glm/vec4.hpp"
#include <cassert>
#include <cstring>
#include <optional>

namespace division_engine::core
{
class RenderPassDescriptorBuilder
{
public:
    RenderPassDescriptorBuilder(DivisionContext* context_ptr)
      : _ctx(context_ptr)
      , _desc(DivisionRenderPassDescriptor {
            .capabilities_mask = DIVISION_RENDER_PASS_DESCRIPTOR_CAPABILITY_NONE,
            .color_mask = DIVISION_COLOR_MASK_RGB,
        })
    {
    }

    RenderPassDescriptorBuilder shader(DivisionId shader_id)
    {
        _init_shader = true;
        _desc.shader_program = shader_id;
        return *this;
    }

    RenderPassDescriptorBuilder vertex_buffer(DivisionId vertex_buffer_id)
    {
        _init_vertex_buffer = true;
        _desc.vertex_buffer_id = vertex_buffer_id;
        return *this;
    }

    RenderPassDescriptorBuilder enable_aplha_blending(
        AlphaBlend alpha_blend,
        DivisionAlphaBlendOperation alpha_blend_op)
    {
        DivisionAlphaBlendingOptions blend_options {
            .src = alpha_blend.src_blend(),
            .dst = alpha_blend.dst_blend(),
            .operation = alpha_blend_op,
        };

        if (alpha_blend.with_constant_color())
        {
            glm::vec4 col = alpha_blend.constant_color();
            std::memcpy(blend_options.constant_blend_color, &col, sizeof(glm::vec4));
        }

        _desc.alpha_blending_options = blend_options;
        _desc.capabilities_mask = static_cast<DivisionRenderPassDescriptorCapabilityMask>(
            _desc.capabilities_mask |
            DIVISION_RENDER_PASS_DESCRIPTOR_CAPABILITY_ALPHA_BLEND);

        return *this;
    }

    DivisionId build()
    {
        if (!_init_shader | !_init_vertex_buffer)
        {
            throw Exception {
                "Render pass vertex buffer and shader must be set before build"
            };
        }

        DivisionId pass_desc_id;

        if (!division_engine_render_pass_descriptor_alloc(_ctx, &_desc, &pass_desc_id))
        {
            throw Exception { "Failed to create the render pass" };
        }
        return pass_desc_id;
    }

private:
    bool _init_shader;
    bool _init_vertex_buffer;
    DivisionRenderPassDescriptor _desc;
    DivisionContext* _ctx;
};
}