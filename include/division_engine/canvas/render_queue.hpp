#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "canvas/state.hpp"
#include "division_engine_core/context.h"
#include "division_engine_core/render_pass_instance.h"

namespace division_engine::canvas
{
class RenderQueue
{
public:
    RenderQueue(DivisionContext* context) : _context(context) {}
    RenderQueue(RenderQueue& render_queue) = delete;
    RenderQueue operator=(RenderQueue& render_queue) = delete;

    void enqueue_pass(DivisionRenderPassInstance& pass, uint32_t order);
    void draw(State& state);

private:
    std::vector<std::pair<DivisionRenderPassInstance, uint32_t>> _render_passes;
    std::vector<DivisionRenderPassInstance> _sorted_passes;
    DivisionContext* _context;
};
}