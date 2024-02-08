#include "division_engine/core/context_helper.hpp"
#include "division_engine/core/types.hpp"
#include "division_engine_core/context.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/vertex_buffer.h"

#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <glm/fwd.hpp>

#include <division_engine/core/core_runner.hpp>
#include <division_engine/core/vertex_data.hpp>

using namespace division_engine::core;

struct Vert
{
    glm::vec2 vertex_position;
    glm::vec2 uv;

    static constexpr DivisionVertexAttributeSettings vertex_attributes[] = {
        make_vertex_attribute<decltype(Vert::vertex_position)>(0),
        make_vertex_attribute<decltype(Vert::uv)>(1)
    };
} __attribute__((__packed__));

struct Inst
{
    glm::vec2 size;
    glm::vec2 position;
    glm::vec4 color;
    glm::vec4 trbl_border_radius;

    static constexpr DivisionVertexAttributeSettings vertex_attributes[] = {
        make_vertex_attribute<decltype(Inst::size)>(2),
        make_vertex_attribute<decltype(Inst::position)>(3),
        make_vertex_attribute<decltype(Inst::color)>(4),
        make_vertex_attribute<decltype(Inst::trbl_border_radius)>(5),
    };
} __attribute__((__packed__));

struct MyLifecycleManager
{
    MyLifecycleManager(DivisionContext* context)
    {
        ContextHelper helper { context };
        _shader_id = helper.create_bundled_shader(
            std::filesystem::path { "resources" } / "shaders" / "canvas" / "rect");

        {
            _screen_uniform_id = helper.create_uniform<glm::vec4>();
            auto screen_uniform = helper.get_uniform_data<glm::vec2>(_screen_uniform_id);
            *screen_uniform.data_ptr = glm::vec2 {
                context->renderer_context->frame_buffer_width,
                context->renderer_context->frame_buffer_height,
            };
        }

        const auto& verts = std::array {
            Vert {
                .vertex_position = glm::vec2(0., 1.),
                .uv = glm::vec2(0., 1.),
            },
            Vert {
                .vertex_position = glm::vec2(0., 0.),
                .uv = glm::vec2(0., 0.),
            },
            Vert {
                .vertex_position = glm::vec2(1., 0.),
                .uv = glm::vec2(1., 0.),
            },
            Vert {
                .vertex_position = glm::vec2(1., 1.),
                .uv = glm::vec2(1., 1.),
            },
        };

        const auto& instances = std::array {
            Inst {
                .position = glm::vec2 { 512, 512 },
                .size = glm::vec2 { 100, 100 },
                .color = glm::vec4 { 1, 1, 0, 1 },
                .trbl_border_radius = glm::vec4 { 0 },
            },
        };

        const auto& indices = std::array { 0, 1, 2, 2, 3, 0 };

        VertexBufferSize buffer_size {
            .vertex_count = static_cast<uint32_t>(verts.size()),
            .index_count = 6,
            .instance_count = static_cast<uint32_t>(instances.size()),
        };

        _vertex_buffer_id = helper.create_vertex_buffer<Vert, Inst>(
            buffer_size, Topology::DIVISION_TOPOLOGY_TRIANGLES);
        {
            auto buffer_data =
                helper.get_vertex_buffer_data<Vert, Inst>(_vertex_buffer_id);
            std::copy(
                std::begin(verts),
                std::end(verts),
                buffer_data.per_vertex_data().begin());

            std::copy(
                std::begin(instances),
                std::end(instances),
                buffer_data.per_instance_data().begin());

            std::copy(
                std::begin(indices), std::end(indices), buffer_data.index_data().begin());
        }

        auto render_pass_desc_id =  helper.render_pass_descriptor_builder()
            .shader(_shader_id)
            .vertex_buffer(_vertex_buffer_id)
            .build();
    }

    ~MyLifecycleManager() { std::cout << "Lifecycle manager was destroyed" << std::endl; }

    void draw(DivisionContext* context) {}

    void error(DivisionContext* context, int32_t errorCode, const char* errorMessage)
    {
        std::cerr << "Error code: " << errorCode << ". Error message: " << errorMessage
                  << std::endl;
    }

private:
    DivisionId _shader_id;
    DivisionId _vertex_buffer_id;
    DivisionId _screen_uniform_id;
};

struct MyLifecycleManagerBuilder
{
    using manager_type = MyLifecycleManager;

    manager_type* build(DivisionContext* context)
    {
        std::cout << "Hello from lifecycle builder" << std::endl;

        return new MyLifecycleManager { context };
    }
};

int main(int argc, char** argv)
{
    division_engine::core::CoreRunner coreRunner {
        std::string { "Hello division cpp" },
        { 512, 512 },
    };

    MyLifecycleManagerBuilder builder {};
    coreRunner.run(builder);
}