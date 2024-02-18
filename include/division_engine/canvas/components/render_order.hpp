#pragma once

#include <cstdint>

namespace division_engine::canvas::components
{
struct RenderOrder
{
    uint32_t order;

    int compare(const RenderOrder& other) const
    {
        return static_cast<int>(order) - static_cast<int>(other.order);
    }
};
}