#pragma once

#include "exception.hpp"

#include <division_engine_core/types/id.h>
#include <division_engine_core/vertex_buffer.h>

#include <span>

struct DivisionContext;

namespace division_engine::core
{

template<typename TVertexData, typename TInstanceData>
struct VertexBufferData
{
    VertexBufferData(const VertexBufferData&) = delete;
    VertexBufferData(VertexBufferData&&) = delete;
    VertexBufferData& operator=(const VertexBufferData&) = delete;
    VertexBufferData& operator=(VertexBufferData&&) = delete;

    VertexBufferData(DivisionContext* context_ptr, DivisionId vertex_buffer_id)
      : context_ptr(context_ptr)
      , vertex_buffer_id(vertex_buffer_id)
      , borrowed_data()
    {
        if (!division_engine_vertex_buffer_borrow_data(
                context_ptr, vertex_buffer_id, &borrowed_data
            ))
        {
            throw Exception { "Failed to get vertex buffer data" };
        }
    }

    ~VertexBufferData()
    {
        division_engine_vertex_buffer_return_data(
            context_ptr, vertex_buffer_id, &borrowed_data
        );
    }

    std::span<TVertexData> per_vertex_data()
    {
        return std::span {
            static_cast<TVertexData*>(borrowed_data.vertex_data_ptr),
            borrowed_data.size.vertex_count,
        };
    }

    std::span<TInstanceData> per_instance_data()
    {
        return std::span { static_cast<TInstanceData*>(borrowed_data.instance_data_ptr),
                           borrowed_data.size.instance_count };
    }

    std::span<uint32_t> index_data()
    {
        return std::span {
            static_cast<uint32_t*>(borrowed_data.index_data_ptr),
            borrowed_data.size.index_count,
        };
    }

    DivisionContext* context_ptr;
    DivisionId vertex_buffer_id;
    DivisionVertexBufferBorrowedData borrowed_data;
};

}