#pragma once

#include "components/rect_instance.hpp"
#include "components/render_order.hpp"
#include "components/render_texture.hpp"

#include "division_engine/core/context.hpp"

#include "state.hpp"

#include <division_engine_core/types/render_pass_instance.h>
#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace division_engine::canvas
{
struct RectVertex
{
    glm::vec2 vertex_position;
    glm::vec2 uv;

    static constexpr auto vertex_attributes = std::array {
        core::make_vertex_attribute<decltype(RectVertex::vertex_position)>(0),
        core::make_vertex_attribute<decltype(RectVertex::uv)>(1)
    };
} __attribute__((__packed__));

class RectDrawer
{
public:
    using RenderTexture = components::RenderTexture;
    using RectInstance = components::RectInstance;
    using RenderOrder = components::RenderOrder;

    static const size_t DEFAULT_RECT_CAPACITY = 64;
    static const size_t SCREEN_SIZE_UNIFORM_LOCATION = 1;
    static const size_t TEXTURE_LOCATION = 0;

    static constexpr auto RECT_VERTICES = std::array {
        RectVertex {
            .vertex_position = glm::vec2(0., 1.),
            .uv = glm::vec2(0., 1.),
        },
        RectVertex {
            .vertex_position = glm::vec2(0., 0.),
            .uv = glm::vec2(0., 0.),
        },
        RectVertex {
            .vertex_position = glm::vec2(1., 0.),
            .uv = glm::vec2(1., 0.),
        },
        RectVertex {
            .vertex_position = glm::vec2(1., 1.),
            .uv = glm::vec2(1., 1.),
        },
    };
    static constexpr auto RECT_INDICES = std::array { 0u, 1u, 2u, 2u, 3u, 0u };

    RectDrawer(const RectDrawer&) = default;
    RectDrawer(RectDrawer&&) = delete;
    RectDrawer& operator=(const RectDrawer&) = default;
    RectDrawer& operator=(RectDrawer&&) = delete;
    RectDrawer(State& state, size_t rect_capacity = DEFAULT_RECT_CAPACITY);
    ~RectDrawer();

    void update(State& state);

private:
    flecs::query<RectInstance, RenderOrder, RenderTexture> _query;
    std::vector<DivisionIdWithBinding> _textures_heap;
    core::Context _ctx_helper;
    DivisionIdWithBinding _screen_size_uniform;

    DivisionId _shader_id;
    DivisionId _vertex_buffer_id;
    DivisionId _render_pass_descriptor_id;

    uint32_t _instance_capacity;

    static DivisionId
    make_vertex_buffer(core::Context& context_helper, uint32_t instance_capacity);

    DivisionRenderPassInstance make_render_pass_instance(
        DivisionIdWithBinding* texture_ptr,
        size_t first_instance,
        size_t instance_count
    );
};
}