#include "core/core_runner.hpp"

#include <division_engine_core/renderer.h>

namespace division_engine::core
{
CoreRunner::CoreRunner(std::string window_title, glm::uvec2 window_size)
  : _ctx(new DivisionContext)
  , _window_title(std::move(window_title))
  , _window_size(window_size)
{
    DivisionSettings settings {
        .window_width = _window_size.x,
        .window_height = _window_size.y,
        .window_title = _window_title.c_str(),
    };

    division_engine_context_initialize(&settings, _ctx);
}

CoreRunner::~CoreRunner()
{
    if (_ctx)
    {
        division_engine_context_finalize(_ctx);

        delete _ctx;
    }
}

void CoreRunner::run(const DivisionLifecycle* lifecycle)
{
    division_engine_context_register_lifecycle(_ctx, lifecycle);
    division_engine_renderer_run_loop(_ctx);
}
}