#pragma once

#include <division_engine_core/context.h>
#include <division_engine_core/render_pass_instance.h>
#include <glm/vec4.hpp>

#include <cstdint>
#include <utility>
#include <vector>

namespace division_engine::canvas
{
class RenderQueue
{
public:
    RenderQueue(): _render_passes(), _sorted_passes() {}
    RenderQueue(RenderQueue& render_queue) = delete;
    RenderQueue operator=(RenderQueue& render_queue) = delete;

    void enqueue_pass(const DivisionRenderPassInstance& pass, uint32_t order);
    void draw(DivisionContext* context, const glm::vec4& clear_color);

private:
    std::vector<std::pair<DivisionRenderPassInstance, uint32_t>> _render_passes;
    std::vector<DivisionRenderPassInstance> _sorted_passes;
};
}