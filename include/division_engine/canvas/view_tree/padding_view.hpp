#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/padding.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "glm/common.hpp"
#include "glm/vec2.hpp"
#include <stdexcept>
#include <variant>

namespace division_engine::canvas::view_tree
{
template<View TChild>
struct PaddingView
{
    struct Renderer;

    using renderer_type = Renderer;

    Padding padding;
    TChild child;

    PaddingView(TChild child, Padding padding = Padding::all(0))
      : child(child), padding(padding) {};

    PaddingView&& with_padding(Padding padding)
    {
        this->padding = padding;
        return std::move(*this);
    }
};

template<View TChild>
struct PaddingView<TChild>::Renderer
{
    using view_type = PaddingView<TChild>;
    using child_renderer_type = typename TChild::renderer_type;

    child_renderer_type child_renderer;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
      : child_renderer(child_renderer_type { state, render_manager, view.child })
    {
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        glm::vec2 padded_size { view.padding.left + view.padding.right,
                                view.padding.top + view.padding.bottom };

        BoxConstraints child_constraints {
            .min_size = glm::min(glm::vec2 { 0 }, constraints.min_size - padded_size),
            .max_size = constraints.max_size - padded_size,
        };

        Size child_size = child_renderer.layout(child_constraints, view.child);

        Size result_size = Size::unconstrainted();
        if (!child_size.width_unconstrainted())
        {
            result_size.width = child_size.width + padded_size.x;
        }
        if (!child_size.height_unconstrainted())
        {
            result_size.height = child_size.height + padded_size.y;
        }

        return result_size;
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        const auto& padding = view.padding;

        auto rect_size = rect.size();
        rect = Rect::from_bottom_left(
            glm::vec2 { rect.left() + padding.left, rect.bottom() + padding.bottom },
            glm::vec2 {
                rect_size.x - (padding.left + padding.right),
                rect_size.y - (padding.bottom + padding.top),
            }
        );

        child_renderer.render(state, render_manager, rect, view.child);
    }
};
}