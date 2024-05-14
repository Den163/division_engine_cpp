#pragma once

#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/components/render_bounds.hpp"
#include "division_engine/canvas/components/render_texture.hpp"
#include "division_engine/canvas/components/renderable_rect.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size_variant.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/color.hpp"
#include "flecs.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <tuple>

namespace division_engine::canvas::view_tree
{
struct DecoratedBoxViewRenderer;

struct DecoratedBoxView
{
    using renderer = DecoratedBoxViewRenderer;

    glm::vec4 background_color = color::WHITE;
    BorderRadius border_radius = BorderRadius::all(0);
};

struct DecoratedBoxViewRenderer
{
    flecs::entity_t renderable_id;

    static DecoratedBoxViewRenderer
    create(State& state, RenderManager& render_manager, const DecoratedBoxView& view)
    {
        using namespace components;

        auto batch_entity =
            state.world.entity().set(RenderTexture { state.white_texture_id });

        auto renderable_id = render_manager.create_renderer(
            state,
            RectDrawer::renderable_type {
                RenderableRect {
                    .color = view.background_color,
                    .border_radius = view.border_radius,
                },
                RenderBounds { Rect::from_center(glm::vec2 { 0 }, glm::vec2 { 0 }) } },
            batch_entity.id()
        );

        DecoratedBoxViewRenderer render { .renderable_id = renderable_id };
        return render;
    }

    SizeVariant layout(const BoxConstraints& constraints, const DecoratedBoxView& view)
    {
        return SizeVariant::fill();
    }

    void render(
        State& state,
        RenderManager& render_manager,
        Rect& rect,
        const DecoratedBoxView& view
    )
    {
        using namespace components;

        auto e = flecs::entity { state.world, renderable_id };

        auto& renderable = *e.get_mut<RenderableRect>();
        renderable.color = view.background_color;
        renderable.border_radius = view.border_radius;

        auto& bounds = *e.get_mut<RenderBounds>();
        bounds = rect;
    }
};
}