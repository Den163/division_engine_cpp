#include "canvas/rect_drawer.hpp"
#include "core/alpha_blend.hpp"
#include "core/render_pass_instance_builder.hpp"
#include "core/types.hpp"
#include "division_engine_core/render_pass_descriptor.h"
#include "division_engine_core/render_pass_instance.h"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/iter.hpp"

#include <bits/ranges_algo.h>
#include <bits/ranges_algobase.h>
#include <division_engine/canvas/components/rect_instance.hpp>
#include <division_engine/canvas/components/render_texture.hpp>
#include <division_engine/core/context_helper.hpp>
#include <division_engine/core/render_pass_instance_builder.hpp>

#include <algorithm>
#include <filesystem>
#include <iterator>

#include <ranges>
#include <vector>

namespace division_engine::canvas
{
const size_t WHITE_TEXTURE_INDEX = 0;

using components::RectInstance;
using components::RenderTexture;

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
    auto overall_instance_count = 0;
    std::vector<DivisionRenderPassInstance> render_passes;
    {
        auto data = _ctx_helper.borrow_vertex_buffer_data<RectVertex, RectInstance>(
            _vertex_buffer_id
        );

        auto instances = data.per_instance_data();

        const auto& filter =
            state.world.filter_builder<RectInstance, RenderTexture>()
                .term<RenderTexture>()
                .up(flecs::IsA)
                .instanced()
                .build();


        filter.iter(
            [&](flecs::iter& it, RectInstance* rects, RenderTexture* tex_ptr)
            {
                auto lower_bound = std::lower_bound(
                    _textures_heap.begin(),
                    _textures_heap.end(),
                    tex_ptr->texture_id,
                    [](const auto& x, auto y) { return x.id < y; }
                );
                auto first_instance = overall_instance_count;
                auto instance_count = it.count();

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

                auto texture_index = std::distance(_textures_heap.begin(), lower_bound);

                std::ranges::copy_n(rects, instance_count, instances.data());
                overall_instance_count += instance_count;

                render_passes.push_back(make_render_pass_instance(
                    &_textures_heap[texture_index], first_instance, instance_count
                ));
            }
        );
    }

    _ctx_helper.draw_render_passes(render_passes, glm::vec4 { 1 });
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