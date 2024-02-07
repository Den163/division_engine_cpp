#pragma once

#include <glm/glm.hpp>

#include <division_engine_core/context.h>
#include <division_engine_core/division_lifecycle.h>
#include <division_engine_core/renderer.h>
#include <division_engine_core/settings.h>
#include <string>

#include <division_engine/core/lifecycle_manager.hpp>

namespace division_engine::core
{
struct CoreRunner
{
    CoreRunner(std::string window_title, glm::uvec2 window_size)
      : _window_title(std::move(window_title))
      , _window_size(window_size)
      , _ctx(nullptr)
    {
    }

    ~CoreRunner()
    {
        if (_ctx)
        {
            division_engine_context_finalize(_ctx);

            delete _ctx;
        }
    }

    template<LifecycleManagerBuilder T>
    void run(T& lifecycle_manager_builder)
    {
        using ManagerType = typename T::manager_type;

        _ctx = new DivisionContext;
        DivisionSettings settings {
            .window_width = _window_size.x,
            .window_height = _window_size.y,
            .window_title = _window_title.c_str(),
        };
        division_engine_context_initialize(&settings, _ctx);
        _ctx->user_data = &lifecycle_manager_builder;

        DivisionLifecycle lifecycle {
            .init_callback = builder_init_callback<T>,
            .draw_callback = draw_callback<ManagerType>,
            .free_callback = cleanup<ManagerType>,
            .error_callback = error_callback<ManagerType>,
        };
        division_engine_context_register_lifecycle(_ctx, &lifecycle);
        division_engine_renderer_run_loop(_ctx);
    }

private:
    DivisionContext* _ctx;
    std::string _window_title;
    glm::uvec2 _window_size;

    template<LifecycleManagerBuilder T>
    static void builder_init_callback(DivisionContext* context)
    {
        auto* builder = static_cast<T*>(context->user_data);
        auto* manager = builder->build(context);

        context->user_data = manager;
    }

    template<LifecycleManager T>
    static void cleanup(DivisionContext* context)
    {
        T* manager = static_cast<T*>(context->user_data);
        delete manager;
    }

    template<LifecycleManager T>
    static void draw_callback(DivisionContext* context)
    {
        T& manager = *static_cast<T*>(context->user_data);
        manager.draw(context);
    }

    template<LifecycleManager T>
    static void error_callback(
      DivisionContext* context,
      int32_t error_code,
      const char* error_message)
    {
        T& manager = *static_cast<T*>(context->user_data);
        manager.error(context, error_code, error_message);
    }
};
} // namespace division_engine::core