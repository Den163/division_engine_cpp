#include "division_engine/canvas/render_queue.hpp"
#include "canvas/state.hpp"
#include "division_engine_core/color.h"
#include "division_engine_core/render_pass_instance.h"

#include <algorithm>
#include <bits/ranges_algo.h>
#include <ranges>

namespace division_engine::canvas
{
void RenderQueue::enqueue_pass(DivisionRenderPassInstance& pass, uint32_t order)
{
    _render_passes.push_back({ pass, order });
}

void RenderQueue::draw(State& state)
{
    std::ranges::sort(
        _render_passes, [](const auto& x, const auto& y) { return y.second - x.second; }
    );

    const auto& passes_view = std::transform(
        _render_passes.begin(),
        _render_passes.end(),
        _sorted_passes.begin(),
        [](const auto& pair) { return pair.first; }
    );

    const auto& clear_color = state.clear_color;
    division_engine_render_pass_instance_draw(
        _context,
        reinterpret_cast<DivisionColor*>(&state.clear_color),
        _sorted_passes.data(),
        static_cast<uint32_t>(_sorted_passes.size())
    );
}
}