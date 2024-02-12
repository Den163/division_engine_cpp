#include "canvas/render_queue.hpp"
#include "canvas/state.hpp"

#include <division_engine_core/color.h>
#include <division_engine_core/context.h>
#include <division_engine_core/render_pass_instance.h>

#include <algorithm>
#include <ranges>

namespace division_engine::canvas
{
void RenderQueue::enqueue_pass(const DivisionRenderPassInstance& pass, uint32_t order)
{
    _render_passes.push_back({ pass, order });
}

void RenderQueue::draw(DivisionContext* context, const glm::vec4& clear_color)
{
    std::ranges::sort(
        _render_passes, [](const auto& x, const auto& y) { return y.second - x.second; }
    );
    
    _sorted_passes.resize(_render_passes.size());
    std::transform(
        _render_passes.begin(),
        _render_passes.end(),
        _sorted_passes.begin(),
        [](const auto& pair) { return pair.first; }
    );

    division_engine_render_pass_instance_draw(
        context,
        reinterpret_cast<const DivisionColor*>(&clear_color),
        _sorted_passes.data(),
        static_cast<uint32_t>(_sorted_passes.size())
    );

    _render_passes.clear();
    _sorted_passes.clear();
}
}