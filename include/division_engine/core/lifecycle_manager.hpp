#pragma once

#include <concepts>
#include <division_engine_core/context.h>

namespace division_engine::core
{

template<typename T>
concept LifecycleManager = requires(
  T manager,
  DivisionContext* context,
  int32_t error_code,
  const char* error_message) {
    {
        manager.draw(context)
    };
    {
        manager.error(context, error_code, error_message)
    };
};

template<typename T>
concept LifecycleManagerBuilder =
  LifecycleManager<typename T::manager_type> && requires(T builder, DivisionContext* ctx) {
      {
          builder.build(ctx)
      } -> std::same_as<typename T::manager_type*>;
  };
}