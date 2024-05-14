#pragma once

namespace division_engine::canvas::view_tree
{
template<typename TView>
using renderer_of_view_t = typename TView::renderer_type;

template<typename TRenderer>
using view_of_renderer_t = typename TRenderer::view_type;
}