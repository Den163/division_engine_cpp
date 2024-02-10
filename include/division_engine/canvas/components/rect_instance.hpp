#include <division_engine_core/vertex_buffer.h>

#include <division_engine/core/vertex_data.hpp>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace division_engine::canvas::components
{
/*
    Rect instance is an ECS component and a Rect Drawer's vertex data
*/
struct RectInstance
{
    glm::vec2 size;
    glm::vec2 position;
    glm::vec4 color;
    glm::vec4 trbl_border_radius;

    static constexpr DivisionVertexAttributeSettings vertex_attributes[] = {
        core::make_vertex_attribute<decltype(RectInstance::size)>(2),
        core::make_vertex_attribute<decltype(RectInstance::position)>(3),
        core::make_vertex_attribute<decltype(RectInstance::color)>(4),
        core::make_vertex_attribute<decltype(RectInstance::trbl_border_radius)>(5),
    };
} __attribute__((__packed__));
}