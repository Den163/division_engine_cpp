#include "canvas/rect_drawer.hpp"

#include "canvas/components/render_batch.hpp"
#include "core/alpha_blend.hpp"
#include "core/context.hpp"
#include "core/render_pass_instance_builder.hpp"

#include "canvas/components/render_texture.hpp"
#include "utility/algorithm.hpp"

#include <division_engine_core/render_pass_descriptor.h>
#include <division_engine_core/render_pass_instance.h>
#include <division_engine_core/vertex_buffer.h>

#include <algorithm>
#include <filesystem>
#include <iterator>

#include <ranges>
#include <vector>

namespace division_engine::canvas
{
using namespace components;

RectDrawer::RectDrawer(State& state, size_t rect_capacity)
  : _texture_bindings({ DivisionIdWithBinding {
        .id = state.white_texture_id,
        .shader_location = TEXTURE_LOCATION,
    } })
  , _ctx(state.context)
  , _screen_size_uniform(DivisionIdWithBinding {
        .id = state.screen_size_uniform_id,
        .shader_location = SCREEN_SIZE_UNIFORM_LOCATION,
    })
  , _vertex_buffer_id(make_vertex_buffer(_ctx, rect_capacity))
  , _instance_capacity(rect_capacity)
  , _resources_owner(true)
{
    using path = std::filesystem::path;

    _shader_id =
        _ctx.create_bundled_shader(path { "resources" } / "shaders" / "canvas" / "rect");

    _render_pass_descriptor_id =
        _ctx.render_pass_descriptor_builder()
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

    _query =
        state.world
            .query_builder<
                const RenderBounds,
                const RenderableRect,
                const RenderOrder,
                const RenderTexture>()
            .term<RenderBatch>()
            .up(flecs::IsA)
            .term<RenderTexture>()
            .up(flecs::IsA)
            .order_by<RenderOrder>([](auto, const auto* x, auto, const auto* y)
                                   { return x->compare(*y); })
            .instanced()
            .build();
}

RectDrawer::~RectDrawer()
{
    if (!_resources_owner)
        return;

    _ctx.delete_shader(_shader_id);
    _ctx.delete_vertex_buffer(_vertex_buffer_id);
}

void RectDrawer::update(State& state)
{
    auto overall_instance_count = 0;

    const auto needed_capacity = _query.count();
    if (_instance_capacity < needed_capacity)
    {
        _ctx.resize_vertex_buffer(
            _vertex_buffer_id,
            DivisionVertexBufferSize {
                .vertex_count = RECT_VERTICES.size(),
                .index_count = RECT_INDICES.size(),
                .instance_count = static_cast<uint32_t>(needed_capacity) }
        );
        _instance_capacity = needed_capacity;
    }

    auto data =
        _ctx.borrow_vertex_buffer_data<RectVertex, RectInstance>(_vertex_buffer_id);
    auto instances = data.per_instance_data();

    _query.iter(
        [&](flecs::iter& it,
            const RenderBounds* render_bounds,
            const RenderableRect* rects,
            const RenderOrder* ord_ptr,
            const RenderTexture* tex_ptr)
        {
            auto new_texture_binding = DivisionIdWithBinding {
                .id = tex_ptr->texture_id,
                .shader_location = TEXTURE_LOCATION,
            };

            auto insert_pos_iter = utility::algorithm::sorted_insert(
                _texture_bindings,
                new_texture_binding,
                [](const auto& x, const auto& y) { return x.id < y.id; }
            );

            const auto texture_index =
                std::distance(_texture_bindings.begin(), insert_pos_iter);

            const auto first_instance = overall_instance_count;
            const auto rect_count = it.count();

            auto batch_instances = instances.subspan(first_instance, rect_count);
            uint32_t order = 0;

            for (auto i : it)
            {
                const auto& rect = rects[i];
                const auto& bounds = render_bounds[i].value;

                batch_instances[i] = RectInstance {
                    .size = bounds.size(),
                    .position = glm::vec2 { bounds.left(), bounds.bottom() },
                    .color = rect.color,
                    .trbl_border_radius = rect.border_radius.top_left_right_bottom
                };

                order = ord_ptr[i].order;
            }

            overall_instance_count += static_cast<int>(rect_count);

            state.render_queue.enqueue_pass(
                make_render_pass_instance(
                    &_texture_bindings[texture_index], first_instance, rect_count
                ),
                order
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