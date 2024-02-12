#pragma once

#include "exception.hpp"
#include "types.hpp"

#include <division_engine_core/context.h>
#include <division_engine_core/uniform_buffer.h>

namespace division_engine::core
{
template<typename T>
struct UniformData
{
    UniformData(DivisionContext* context_ptr, DivisionId uniform_id)
      : context_ptr(context_ptr)
      , uniform_id(uniform_id)
    {
        DivisionUniformBufferDescriptor desc { .data_bytes = sizeof(T) };
        data_ptr = static_cast<T*>(
            division_engine_uniform_buffer_borrow_data_pointer(context_ptr, uniform_id));

        if (data_ptr == nullptr)
        {
            throw Exception { "Failed to get uniform data" };
        }
    }

    ~UniformData()
    {
        division_engine_uniform_buffer_return_data_pointer(
            context_ptr, uniform_id, data_ptr);
    }

    T* data_ptr;
    DivisionContext* context_ptr;
    DivisionId uniform_id;
};
}