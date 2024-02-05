#pragma once

#include <concepts>
#include <division_engine_core/context.h>

namespace division_engine::core
{

template<typename T>
concept LifecycleManager = requires(
  T manager,
  DivisionContext* context,
  int32_t errorCode,
  const char* errorMessage) {
    {
        manager.draw(context)
    };
    {
        manager.error(context, errorCode, errorMessage)
    };
    {
        manager.cleanup(context)
    };
};

template<typename T>
concept LifecycleManagerBuilder =
  LifecycleManager<typename T::managerType> && requires(T builder, DivisionContext* ctx) {
      {
          builder.build(ctx)
      } -> std::same_as<typename T::managerType>;
  };
}