#pragma once

#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/components/render_bounds.hpp"
#include "division_engine/canvas/components/render_texture.hpp"
#include "division_engine/canvas/components/renderable_rect.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/color.hpp"
#include "flecs.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <tuple>

namespace division_engine::canvas::view_tree
{
struct DecoratedBoxView
{
    struct Renderer;

    glm::vec4 background_color = color::WHITE;
    BorderRadius border_radius = BorderRadius::all(0);
};

struct DecoratedBoxView::Renderer
{
    using view_type = DecoratedBoxView;

    flecs::entity_t renderable_id;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
    {
        using namespace components;

        auto batch_entity =
            state.world.entity().set(RenderTexture { state.white_texture_id });

        renderable_id = render_manager.create_renderer(
            state,
            RectDrawer::renderable_type {
                RenderableRect {
                    .color = view.background_color,
                    .border_radius = view.border_radius,
                },
                RenderBounds { Rect::from_center(glm::vec2 { 0 }, glm::vec2 { 0 }) } },
            batch_entity.id()
        );
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        return Size::unconstrainted();
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        using namespace components;

        flecs::entity entity { state.world, renderable_id };

        auto& renderable = *entity.get_mut<RenderableRect>();
        renderable.color = view.background_color;
        renderable.border_radius = view.border_radius;

        auto& bounds = *entity.get_mut<RenderBounds>();
        bounds = rect;
    }
};
}