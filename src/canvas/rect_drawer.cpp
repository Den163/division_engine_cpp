#include "canvas/rect_drawer.hpp"
#include "core/alpha_blend.hpp"
#include "core/render_pass_instance_builder.hpp"

#include "canvas/components/rect_instance.hpp"
#include "canvas/components/render_texture.hpp"
#include "core/context.hpp"
#include "core/render_pass_instance_builder.hpp"
#include "division_engine_core/vertex_buffer.h"

#include <division_engine_core/render_pass_descriptor.h>
#include <division_engine_core/render_pass_instance.h>

#include <algorithm>
#include <filesystem>
#include <iterator>

#include <ranges>
#include <vector>

namespace division_engine::canvas
{
using components::RectInstance;
using components::RenderTexture;

RectDrawer::RectDrawer(State& state, size_t rect_capacity)
  : _textures_heap({ DivisionIdWithBinding {
        .id = state.white_texture_id,
        .shader_location = TEXTURE_LOCATION,
    } })
  , _ctx_helper(state.context)
  , _screen_size_uniform(DivisionIdWithBinding {
        .id = state.screen_size_uniform_id,
        .shader_location = SCREEN_SIZE_UNIFORM_LOCATION,
    })
  , _vertex_buffer_id(make_vertex_buffer(_ctx_helper, rect_capacity))
  , _instance_capacity(rect_capacity)
{
    using path = std::filesystem::path;

    _shader_id = _ctx_helper.create_bundled_shader(
        path { "resources" } / "shaders" / "canvas" / "rect"
    );

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

    _query = state.world.query_builder<RectInstance, RenderOrder, RenderTexture>()
                 .term<RenderTexture>()
                 .up(flecs::IsA)
                 .order_by<RenderOrder>(
                     [](auto e, const auto* ex, auto y, const auto* ey)
                     { return static_cast<int>(ex->order) - static_cast<int>(ey->order); }
                 )
                 .instanced()
                 .build();
}

RectDrawer::~RectDrawer()
{
    _ctx_helper.delete_shader(_shader_id);
    _ctx_helper.delete_vertex_buffer(_vertex_buffer_id);
}

void RectDrawer::update(State& state)
{
    auto overall_instance_count = 0;

    auto needed_capacity = _query.count();
    if (_instance_capacity < needed_capacity)
    {
        _ctx_helper.resize_vertex_buffer(
            _vertex_buffer_id,
            DivisionVertexBufferSize {
                .vertex_count = RECT_VERTICES.size(),
                .index_count = RECT_INDICES.size(),
                .instance_count = static_cast<uint32_t>(needed_capacity) }
        );
        _instance_capacity = needed_capacity;
    }

    auto data =
        _ctx_helper.borrow_vertex_buffer_data<RectVertex, RectInstance>(_vertex_buffer_id
        );
    auto instances = data.per_instance_data();

    _query.iter(
        [&](flecs::iter& it,
            RectInstance* rects,
            RenderOrder* ord_ptr,
            RenderTexture* tex_ptr)
        {
            const auto lower_bound = std::lower_bound(
                _textures_heap.begin(),
                _textures_heap.end(),
                tex_ptr->texture_id,
                [](const auto& x, auto y) { return x.id < y; }
            );

            if (lower_bound == _textures_heap.end() ||
                lower_bound->id != tex_ptr->texture_id)
            {
                _textures_heap.insert(
                    lower_bound,
                    DivisionIdWithBinding {
                        .id = tex_ptr->texture_id,
                        .shader_location = TEXTURE_LOCATION,
                    }
                );
            }

            const auto texture_index = std::distance(_textures_heap.begin(), lower_bound);

            const auto first_instance = overall_instance_count;
            const auto rect_count = it.count();
            const auto& batch_rects = std::span { rects, rect_count };

            auto batch_instances = instances.subspan(first_instance, rect_count);

            std::copy(batch_rects.begin(), batch_rects.end(), batch_instances.begin());
            overall_instance_count += static_cast<int>(rect_count);

            state.render_queue.enqueue_pass(
                make_render_pass_instance(
                    &_textures_heap[texture_index], first_instance, rect_count
                ),
                0
            );
        }
    );
}

DivisionId
RectDrawer::make_vertex_buffer(core::Context& context_helper, uint32_t instance_capacity)
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

DivisionRenderPassInstance RectDrawer::make_render_pass_instance(
    DivisionIdWithBinding* texture_ptr,
    size_t first_instance,
    size_t instance_count
)
{
    return core::RenderPassInstanceBuilder { _render_pass_descriptor_id }
        .uniform_fragment_buffers({ &_screen_size_uniform, 1 })
        .uniform_vertex_buffers({ &_screen_size_uniform, 1 })
        .fragment_textures({ texture_ptr, 1 })
        .vertices(RECT_VERTICES.size())
        .indices(RECT_INDICES.size())
        .instances(instance_count, first_instance)
        .build();
}
}