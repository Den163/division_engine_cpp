#pragma once

#include "components/render_order.hpp"
#include "components/render_texture.hpp"
#include "components/renderable_rect.hpp"

#include "components/render_bounds.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/vertex_data.hpp"

#include "state.hpp"

#include <division_engine_core/types/render_pass_instance.h>
#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace division_engine::canvas
{
class RectDrawer
{
public:
    using renderable_type =
        std::tuple<components::RenderableRect, components::RenderBounds>;

    struct RectVertex
    {
        glm::vec2 vertex_position;
        glm::vec2 uv;

        static constexpr auto vertex_attributes = std::array {
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(vertex_position, 0),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(uv, 1),
        };
    } __attribute__((__packed__));

    struct RectInstance
    {
        glm::vec2 size;
        glm::vec2 position;
        glm::vec4 color;
        glm::vec4 trbl_border_radius;

        static constexpr auto vertex_attributes = std::array {
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(size, 2),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(position, 3),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(color, 4),
            DIVISION_DECLARE_VERTEX_ATTRIBUTE(trbl_border_radius, 5),
        };
    } __attribute__((__packed__));

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

    static const size_t DEFAULT_RECT_CAPACITY = 64;
    static const size_t SCREEN_SIZE_UNIFORM_LOCATION = 1;
    static const size_t TEXTURE_LOCATION = 0;

    static constexpr auto RECT_INDICES = std::array { 0u, 1u, 2u, 2u, 3u, 0u };

    RectDrawer(const RectDrawer&) = delete;
    RectDrawer& operator=(const RectDrawer&) = delete;
    RectDrawer& operator=(RectDrawer&&) noexcept = delete;
    
    RectDrawer(RectDrawer&& other) noexcept
      : _query(std::move(other._query))
      , _texture_bindings(std::move(other._texture_bindings))
      , _ctx(other._ctx)
      , _screen_size_uniform(other._screen_size_uniform)
      , _shader_id(other._shader_id)
      , _vertex_buffer_id(other._vertex_buffer_id)
      , _render_pass_descriptor_id(other._render_pass_descriptor_id)
      , _instance_capacity(other._instance_capacity)
      , _resources_owner(true)
    {
        other._resources_owner = false;
    }

    RectDrawer(State& state, size_t rect_capacity = DEFAULT_RECT_CAPACITY);
    ~RectDrawer();

    void update(State& state);

private:
    using RenderTexture = components::RenderTexture;
    using RenderOrder = components::RenderOrder;
    using RenderBounds = components::RenderBounds;
    using RenderableRect = components::RenderableRect;

    flecs::query<
        const RenderBounds,
        const RenderableRect,
        const RenderOrder,
        const RenderTexture>
        _query;

    std::vector<DivisionIdWithBinding> _texture_bindings;
    core::Context _ctx;
    DivisionIdWithBinding _screen_size_uniform;

    DivisionId _shader_id;
    DivisionId _vertex_buffer_id;
    DivisionId _render_pass_descriptor_id;

    uint32_t _instance_capacity;

    bool _resources_owner;

    static DivisionId
    make_vertex_buffer(core::Context& context_helper, uint32_t instance_capacity);

    DivisionRenderPassInstance make_render_pass_instance(
        DivisionIdWithBinding* texture_ptr,
        size_t first_instance,
        size_t instance_count
    );
};
}