#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/components/render_bounds.hpp"
#include "division_engine/canvas/components/renderable_text.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/renderer.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/color.hpp"
#include "flecs/addons/cpp/entity.hpp"
#include "freetype/ftlcdfil.h"

#include <flecs.h>
#include <glm/vec4.hpp>

#include <string>

namespace division_engine::canvas::view_tree
{
struct TextView
{
    struct Renderer;

    std::u16string text {};
    glm::vec4 color = color::WHITE;
    float font_size = components::RenderableText::DEFAULT_FONT_SIZE;
};

struct TextView::Renderer
{
    using view_type = TextView;

    flecs::entity_t renderable_id;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
    {
        using namespace components;
        auto id = render_manager.create_renderer(
            state,
            std::tuple {
                RenderableText {
                    .text = view.text,
                    .color = view.color,
                    .font_size = view.font_size,
                },
                RenderBounds { Rect::from_center(glm::vec2 { 0 }, glm::vec2 { 0 }) },
            }
        );

        renderable_id = id;
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

        auto& text = *entity.get_mut<RenderableText>();
        text.text = view.text;
        text.color = view.color;
        text.font_size = view.font_size;

        auto& bounds = *entity.get_mut<RenderBounds>();
        bounds.value = rect;
    }
};
}