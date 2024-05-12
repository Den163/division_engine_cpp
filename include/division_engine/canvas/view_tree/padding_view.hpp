#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/padding.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size_variant.hpp"
#include "division_engine/canvas/state.hpp"
#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include <stdexcept>
#include <variant>

namespace division_engine::canvas::view_tree
{
template<typename T>
struct PaddingViewRender;

template<typename T>
struct PaddingView
{
    using renderer = PaddingViewRender<typename T::renderer>;

    Padding padding;
    T child;

    PaddingView(T child, Padding padding = Padding::all(0))
      : child(child) {};

    PaddingView&& with_padding(Padding padding)
    {
        this->padding = padding;
        return std::move(*this);
    }
};

template<typename T>
struct PaddingViewRender
{
    T child;

    template<typename S>
    static PaddingViewRender<T>
    create(State& state, RenderManager& render_manager, const PaddingView<S>& view)
    {
        return PaddingViewRender<T> {
            .child = T::create(state, render_manager, view.child),
        };
    }

    template<typename S>
    SizeVariant layout(const BoxConstraints& constraints, const PaddingView<S>& view) 
    { 
        glm::vec2 padded_size {
            view.padding.left + view.padding.right,
            view.padding.top + view.padding.bottom
        };

        BoxConstraints child_constraints {
            .min_size = glm::min(glm::vec2 {0}, constraints.min_size - padded_size),
            .max_size = constraints.max_size - padded_size,
        };

        SizeVariant child_size = child.layout(child_constraints, view.child);

        if (child_size.is_filled()) {
            return SizeVariant::fill();
        }
        else if (child_size.is_fixed()) {
            const auto fixed_size = std::get<SizeVariant::Fixed>(child_size.variant);
            return SizeVariant::fixed(fixed_size.size + padded_size);
        }
        
        throw std::runtime_error { "Unknown child size variant" };
    }

    template<typename S>
    void render(
        State& state,
        RenderManager& render_manager,
        Rect& rect,
        const PaddingView<S>& view
    )
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

        child.render(state, render_manager, rect, view.child);
    }
};
}