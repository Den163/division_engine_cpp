#include "canvas/rect_drawer.hpp"
#include "core/alpha_blend.hpp"
#include "core/render_pass_instance_builder.hpp"
#include "core/types.hpp"
#include "division_engine_core/render_pass_descriptor.h"
#include "division_engine_core/render_pass_instance.h"

#include <division_engine/core/context_helper.hpp>
#include <division_engine/core/render_pass_instance_builder.hpp>

#include <algorithm>
#include <filesystem>
#include <iterator>

#include <ranges>

namespace division_engine::canvas
{
const size_t WHITE_TEXTURE_INDEX = 0;

RectDrawer::RectDrawer(State& state, size_t rect_capacity)
  : _ctx_helper(state.context_helper)
  , _screen_size_uniform(DivisionIdWithBinding {
        .id = state.screen_size_uniform_id,
        .shader_location = SCREEN_SIZE_UNIFORM_LOCATION,
    })
  , _textures_heap({ DivisionIdWithBinding {
        .id = state.white_texture_id,
        .shader_location = TEXTURE_LOCATION,
    } })
{
    using path = std::filesystem::path;

    _instance_capacity = rect_capacity;

    _shader_id = _ctx_helper.create_bundled_shader(
        path { "resources" } / "shaders" / "canvas" / "rect"
    );
    _vertex_buffer_id = make_vertex_buffer(_ctx_helper, rect_capacity);
    _render_pass_descriptor_id =
        _ctx_helper.render_pass_descriptor_builder()
            .shader(_shader_id)
            .vertex_buffer(_vertex_buffer_id)
            .enable_aplha_blending(
                core::AlphaBlend {
                    DIVISION_ALPHA_BLEND_SRC_ALPHA,
                    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA,
                },
                DIVISION_ALPHA_BLEND_OP_ADD
            )
            .build();
}

RectDrawer::~RectDrawer()
{
    _ctx_helper.delete_shader(_shader_id);
    _ctx_helper.delete_vertex_buffer(_vertex_buffer_id);
}

void RectDrawer::update(State& state)
{
    auto count = 0;
    {
        auto data = _ctx_helper.borrow_vertex_buffer_data<RectVertex, RectInstance>(
            _vertex_buffer_id
        );

        auto instances = data.per_instance_data();

        state.world.each([&count, &instances](RectInstance& rect)
                         { instances[count++] = rect; });
    }

    auto pass =
        core::RenderPassInstanceBuilder { _render_pass_descriptor_id }
            .uniform_fragment_buffers({ &_screen_size_uniform, 1 })
            .uniform_vertex_buffers({ &_screen_size_uniform, 1 })
            .fragment_textures({ &_textures_heap[WHITE_TEXTURE_INDEX], 1 })
            .vertices(RECT_VERTICES.size())
            .indices(RECT_INDICES.size())
            .instances(count)
            .build();

    _ctx_helper.draw_render_passes({ &pass, 1 }, glm::vec4 { 1 });
}

core::DivisionId RectDrawer::make_vertex_buffer(
    core::ContextHelper& context_helper,
    uint32_t instance_capacity
)
{
    auto id = context_helper.create_vertex_buffer<RectVertex, RectInstance>(
        core::VertexBufferSize {
            .vertex_count = RECT_VERTICES.size(),
            .index_count = RECT_INDICES.size(),
            .instance_count = instance_capacity,
        },
        core::Topology::DIVISION_TOPOLOGY_TRIANGLES
    );

    auto data = context_helper.borrow_vertex_buffer_data<RectVertex, RectInstance>(id);

    std::ranges::copy(RECT_VERTICES, data.per_vertex_data().data());
    std::ranges::copy(RECT_INDICES, data.index_data().data());

    return id;
}
}