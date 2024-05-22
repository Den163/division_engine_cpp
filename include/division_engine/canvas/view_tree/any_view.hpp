#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>

namespace division_engine::canvas::view_tree
{
struct AnyView
{
    using renderer_ptr_factory_func_t =
        std::function<std::shared_ptr<void>(State&, RenderManager&, const void* view)>;
    using layout_func_t = std::function<
        Size(void* this_renderer, const BoxConstraints& constraints, const void* view)>;
    using render_func_t = std::function<void(
        void* this_renderer,
        State& state,
        RenderManager& render_manager,
        Rect& rect,
        const void* view
    )>;

    struct Renderer;

    std::shared_ptr<void> view_ptr;
    renderer_ptr_factory_func_t renderer_ctr_func;
    layout_func_t layout_func;
    render_func_t render_func;

    template<View V>
    AnyView(V v)
      : view_ptr(std::make_shared<V>(v))
      , renderer_ctr_func(
            [](State& state, RenderManager& render_manager, const void* view)
            {
                return std::make_shared<typename V::Renderer>(typename V::Renderer {
                    state, render_manager, *static_cast<const V*>(view) });
            }
        )
      , layout_func(
            [](void* this_renderer, const BoxConstraints& constraints, const void* view)
            {
                auto func = std::mem_fn(&V::Renderer::layout);
                return func(
                    *static_cast<typename V::Renderer*>(this_renderer),
                    constraints,
                    *static_cast<const V*>(view)
                );
            }
        )
      , render_func(
            [](void* this_renderer,
               State& state,
               RenderManager& render_manager,
               Rect& rect,
               const void* view)
            {
                auto func = std::mem_fn(&V::Renderer::render);
                func(
                    *static_cast<typename V::Renderer*>(this_renderer),
                    state,
                    render_manager,
                    rect,
                    *static_cast<const V*>(view)
                );
            }
        )
    {
    }
};

struct AnyView::Renderer
{
    std::shared_ptr<void> renderer_ptr;

    Renderer(State& state, RenderManager& render_manager, const AnyView& view)
      : renderer_ptr(view.renderer_ctr_func(state, render_manager, view.view_ptr.get()))
    {
    }

    Size layout(const BoxConstraints& constraints, const AnyView& view)
    {
        return view.layout_func(renderer_ptr.get(), constraints, view.view_ptr.get());
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const AnyView& view)
    {
        view.render_func(
            renderer_ptr.get(), state, render_manager, rect, view.view_ptr.get()
        );
    }
};

}