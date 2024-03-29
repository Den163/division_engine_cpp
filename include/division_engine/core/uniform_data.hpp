#pragma once

#include "exception.hpp"

#include <division_engine_core/uniform_buffer.h>
#include <division_engine_core/types/id.h>

struct DivisionContext;

namespace division_engine::core
{
template<typename T>
struct UniformData
{
    UniformData() = delete;
    UniformData(UniformData&) = delete;
    UniformData(UniformData&&) = delete;
    UniformData& operator=(const UniformData&) = delete;
    UniformData& operator=(UniformData&&) = delete;

    UniformData(DivisionContext* context_ptr, DivisionId uniform_id)
      : data_ptr(static_cast<T*>(
            division_engine_uniform_buffer_borrow_data_pointer(context_ptr, uniform_id)
        ))
      , context_ptr(context_ptr)
      , uniform_id(uniform_id)
    {

        if (data_ptr == nullptr)
        {
            throw Exception { "Failed to get uniform data" };
        }
    }

    ~UniformData()
    {
        division_engine_uniform_buffer_return_data_pointer(
            context_ptr, uniform_id, data_ptr
        );
    }

    T* data_ptr;
    DivisionContext* context_ptr;
    DivisionId uniform_id;
};
}