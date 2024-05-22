#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "glm/ext/vector_float2.hpp"

#include <glm/common.hpp>

namespace division_engine::canvas::view_tree
{
template<View TChild>
struct SizeView
{
    struct Renderer;

    TChild child;
    Size size;

    SizeView(TChild child, Size size = Size::unconstrainted())
      : child(child)
      , size(size)
    {
    }

    SizeView<TChild> with_size(Size new_size) const
    {
        return SizeView(this->child, new_size);
    }
};

template<View TChild>
struct SizeView<TChild>::Renderer
{
    using view_type = SizeView<TChild>;
    using child_renerer_type = typename TChild::Renderer;

    child_renerer_type child_renderer;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
      : child_renderer(child_renerer_type(state, render_manager, view.child))
    {
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        return glm::clamp(
            glm::vec2 { view.size }, constraints.min_size, constraints.max_size
        );
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        child_renderer.render(state, render_manager, rect, view.child);
    }
};
}