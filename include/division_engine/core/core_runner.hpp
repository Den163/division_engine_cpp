#include <glm/glm.hpp>

#include <division_engine_core/context.h>
#include <division_engine_core/division_lifecycle.h>
#include <division_engine_core/renderer.h>
#include <division_engine_core/settings.h>
#include <string>

#include "lifecycle_manager.hpp"

namespace division_engine::core
{
struct CoreRunner
{
    CoreRunner(std::string windowTitle, glm::uvec2 windowSize)
      : windowTitle(std::move(windowTitle))
      , windowSize(windowSize)
      , context(nullptr)
    {
    }

    ~CoreRunner()
    {
        if (context)
        {
            division_engine_context_finalize(context);

            delete context;
        }
    }

    template<LifecycleManagerBuilder T>
    void run(T& lifecycleManagerBuilder)
    {
        using ManagerType = typename T::managerType;

        context = new DivisionContext;
        DivisionSettings settings {
            .window_width = windowSize.x,
            .window_height = windowSize.y,
            .window_title = windowTitle.c_str(),
        };
        division_engine_context_initialize(&settings, context);
        context->user_data = &lifecycleManagerBuilder;

        DivisionLifecycle lifecycle {
            .init_callback = builderInitCallback<T>,
            .draw_callback = drawCallback<ManagerType>,
            .free_callback = cleanup<ManagerType>,
            .error_callback = errorCallback<ManagerType>,
        };
        division_engine_context_register_lifecycle(context, &lifecycle);
        division_engine_renderer_run_loop(context);
    }

private:
    DivisionContext* context;

    std::string windowTitle;
    glm::uvec2 windowSize;

    template<LifecycleManagerBuilder T>
    static void builderInitCallback(DivisionContext* context)
    {
        using ManagerType = typename T::managerType;

        auto* builder = static_cast<T*>(context->user_data);
        auto* manager = new ManagerType;
        *manager = builder->build(context);

        context->user_data = manager;
    }

    template<LifecycleManager T>
    static void cleanup(DivisionContext* context)
    {
        T* manager = static_cast<T*>(context->user_data);
        manager->cleanup(context);
        delete manager;
    }

    template<LifecycleManager T>
    static void drawCallback(DivisionContext* context)
    {
        T* manager = static_cast<T*>(context->user_data);
        manager->draw(context);
    }

    template<LifecycleManager T>
    static void
    errorCallback(DivisionContext* context, int32_t errorCode, const char* errorMessage)
    {
        T* manager = static_cast<T*>(context->user_data);
        manager->error(context, errorCode, errorMessage);
    }
};
} // namespace division_engine::core