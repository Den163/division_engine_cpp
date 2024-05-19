#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "division_engine/utility/algorithm.hpp"

#include <tuple>

namespace division_engine::canvas::view_tree
{

template<View... TChildView>
struct StackView
{
    struct Renderer;

    std::tuple<TChildView...> children;

    StackView(TChildView... children) : children(children...) {}
};

template<View... TChildView>
StackView(std::tuple<TChildView...>) -> StackView<TChildView...>;

template<View... TChildView>
struct StackView<TChildView...>::Renderer
{
    using view_type = StackView<TChildView...>;

    std::tuple<typename TChildView::Renderer...> children;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
      : children(utility::algorithm::tuple_transform(
            [&](View auto el)
            {   
                using child_view_t = decltype(el);
                using child_renderer_t = typename child_view_t::Renderer;
                
                return child_renderer_t { state, render_manager, el };
            },
            view.children
        ))
    {
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        return Size::unconstrainted();
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        utility::algorithm::tuples_zip_foreach(
            [&](auto child_renderer, auto child_view)
            { child_renderer.render(state, render_manager, rect, child_view); },
            children,
            view.children
        );
    }
};
}