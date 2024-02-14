#pragma once

#include "lifecycle_manager.hpp"

#include <division_engine_core/context.h>
#include <division_engine_core/division_lifecycle.h>
#include <functional>
#include <glm/glm.hpp>

#include <string>

namespace division_engine::core
{
struct CoreRunner
{
    CoreRunner(const CoreRunner&) = default;
    CoreRunner(CoreRunner&&) = delete;
    CoreRunner& operator=(const CoreRunner&) = default;
    CoreRunner& operator=(CoreRunner&&) = delete;

    CoreRunner(std::string window_title, glm::uvec2 window_size);
    ~CoreRunner();

    template<LifecycleManagerBuilder T>
    void run(T& lifecycle_manager_builder)
    {
        using ManagerType = typename T::manager_type;

        _ctx->user_data = &lifecycle_manager_builder;
        DivisionLifecycle lifecycle {
            .init_callback = build_callback<T>,
            .draw_callback = draw_callback<ManagerType>,
            .free_callback = cleanup<ManagerType>,
            .error_callback = error_callback<ManagerType>,
        };

        run(&lifecycle);
    }

private:
    DivisionContext* _ctx;
    std::string _window_title;
    glm::uvec2 _window_size;

    void run(const DivisionLifecycle* lifecycle);

    template<LifecycleManagerBuilder T>
    static void build_callback(DivisionContext* context)
    {
        T& builder = *static_cast<T*>(context->user_data);
        auto* manager = builder.build(context);

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
        manager.draw();
    }

    template<LifecycleManager T>
    static void error_callback(
        DivisionContext* context,
        int32_t error_code,
        const char* error_message
    )
    {
        T& manager = *static_cast<T*>(context->user_data);
        manager.error(error_code, error_message);
    }
};
}