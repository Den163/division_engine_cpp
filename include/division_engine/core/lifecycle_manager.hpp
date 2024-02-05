#pragma once

#include <concepts>
#include <cstdint>
#include <division_engine_core/context.h>
#include <type_traits>

namespace division_engine::core
{

template<typename T>
concept LifecycleManager =
  requires(T t, DivisionContext* context, int32_t errorCode, const char* errorMessage) {
      {
          t.draw(context)
      };
      {
          t.error(context, errorCode, errorMessage)
      };
      {
          t.cleanup(context)
      };
  };

template<typename T>
concept LifecycleManagerBuilder =
  LifecycleManager<typename T::managerType> && requires(T t, DivisionContext* ctx) {
      {
          t.build(ctx)
      } -> std::same_as<typename T::managerType>;
  };
}