#pragma once

#include "lifecycle_manager.hpp"

#include <division_engine_core/types/division_lifecycle.h>
#include <glm/glm.hpp>

#include <string>

struct DivisionContext;

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
        using manager_type = typename T::manager_type;

        set_context_user_data(_ctx, &lifecycle_manager_builder);
        DivisionLifecycle lifecycle {
            .init_callback =
                [](DivisionContext* ctx)
            {
                T& builder = *static_cast<T*>(get_context_user_data(ctx));
                auto* manager = builder.build(ctx);

                set_context_user_data(ctx, manager);
            },
            .draw_callback =
                [](DivisionContext* ctx)
            {
                auto& manager = *static_cast<manager_type*>(get_context_user_data(ctx));
                manager.draw();
            },
            .free_callback =
                [](DivisionContext* ctx)
            {
                auto* manager = static_cast<manager_type*>(get_context_user_data(ctx));
                delete manager;
            },
            .error_callback =
                [](DivisionContext* ctx, int error_code, const char* error_message)
            {
                auto& manager = *static_cast<manager_type*>(get_context_user_data(ctx));
                manager.error(error_code, error_message);
            },
        };

        run(&lifecycle);
    }

private:
    DivisionContext* _ctx;
    std::string _window_title;
    glm::uvec2 _window_size;

    void run(const DivisionLifecycle* lifecycle);

    static void* get_context_user_data(DivisionContext* context_ptr);
    static void set_context_user_data(DivisionContext* context_ptr, void* user_data_ptr);
};
}