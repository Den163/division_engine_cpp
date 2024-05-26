#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/view_tree/view.hpp"

#include <functional>
#include <type_traits>

namespace division_engine::canvas::view_tree
{
template<typename T>
constexpr bool can_build_view =
    View<std::invoke_result_t<decltype(&T::operator()), T, State&>>;

template<typename T>
    requires(can_build_view<T>)
struct ViewBuilder
{
    struct Renderer;

    T builder;

    ViewBuilder(T builder)
      : builder(builder)
    {
    }
};

template<typename T>
    requires(can_build_view<T>)
struct ViewBuilder<T>::Renderer
{
    using view_type = ViewBuilder<T>;
    using child_view_t = std::invoke_result_t<decltype(&T::operator()), T, State&>;
    using child_view_renderer_t = typename child_view_t::Renderer;

    child_view_t child;
    child_view_renderer_t child_renderer;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
      : child(view.builder(state))
      , child_renderer(child_view_renderer_t { state, render_manager, child })
    {
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        return child_renderer.layout(constraints, child);
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        child_renderer.render(state, render_manager, rect, child);
    }
};
}