#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size_variant.hpp"
#include "division_engine/utility/algorithm.hpp"

#include "view_traits.hpp"

#include <tuple>

namespace division_engine::canvas::view_tree
{
template<typename... TChildRenderer>
struct StackViewRender;

template<typename... TChildView>
struct StackView
{
    using renderer_type = StackViewRender<renderer_of_view_t<TChildView>...>;

    std::tuple<TChildView...> children;
};

template<typename... TChildView>
StackView(std::tuple<TChildView...>) -> StackView<TChildView...>;

template<typename... TChildRenderer>
struct StackViewRender
{
    using view_type = StackView<view_of_renderer_t<TChildRenderer>...>;

    std::tuple<TChildRenderer...> children;

    static StackViewRender<TChildRenderer...>
    create(State& state, RenderManager& render_manager, const view_type& view)
    {
        return StackViewRender<TChildRenderer...> { utility::algorithm::tuple_transform(
            [&](auto el)
            {
                using child_view_type = decltype(el);
                return child_view_type::renderer_type::create(state, render_manager, el);
            },
            view.children
        ) };
    }

    SizeVariant layout(const BoxConstraints& constraints, const view_type& view)
    {
        return SizeVariant::filled();
    }

    void render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
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