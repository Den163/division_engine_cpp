#include "division_engine/core/context.hpp"
#include "division_engine/core/render_pass_descriptor_builder.hpp"
#include "division_engine/core/render_pass_instance_builder.hpp"
#include "division_engine_core/context.h"
#include "division_engine_core/render_pass_instance.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/vertex_buffer.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iostream>
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

    static constexpr auto vertex_attributes =
        std::array { make_vertex_attribute<decltype(Vert::vertex_position)>(0),
                     make_vertex_attribute<decltype(Vert::uv)>(1) };
} __attribute__((__packed__));

struct Inst
{
    glm::vec2 size;
    glm::vec2 position;
    glm::vec4 color;
    glm::vec4 trbl_border_radius;

    static constexpr auto vertex_attributes = std::array {
        make_vertex_attribute<decltype(Inst::size)>(2),
        make_vertex_attribute<decltype(Inst::position)>(3),
        make_vertex_attribute<decltype(Inst::color)>(4),
        make_vertex_attribute<decltype(Inst::trbl_border_radius)>(5),
    };
} __attribute__((__packed__));

struct MyLifecycleManager
{
    MyLifecycleManager(const MyLifecycleManager&) = default;
    MyLifecycleManager(MyLifecycleManager&&) = delete;
    MyLifecycleManager& operator=(const MyLifecycleManager&) = default;
    MyLifecycleManager& operator=(MyLifecycleManager&&) = delete;

    MyLifecycleManager(DivisionContext* context)
      : _ctx(Context { context })
    {
        using std::filesystem::path;

        _shader_id = _ctx.create_bundled_shader(
            path { "resources" } / "shaders" / "canvas" / "rect"
        );

        {
            _screen_uniform = DivisionIdWithBinding {
                .id = _ctx.create_uniform<glm::vec4>(),
                .shader_location = 1,
            };

            auto screen_uniform = _ctx.get_uniform_data<glm::vec2>(_screen_uniform.id);
            *screen_uniform.data_ptr = _ctx.get_screen_size();
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

        const size_t RECT_SIZE = 100;
        const auto& instances = std::array {
            Inst {
                .size = glm::vec2 { RECT_SIZE },
                .position = glm::vec2 { 0, 0 },
                .color = glm::vec4 { 1, 1, 0, 1 },
                .trbl_border_radius = glm::vec4 { 0 },
            },
        };

        const auto& indices = std::array { 0, 1, 2, 2, 3, 0 };

        VertexBufferSize buffer_size {
            .vertex_count = static_cast<uint32_t>(verts.size()),
            .index_count = static_cast<uint32_t>(indices.size()),
            .instance_count = static_cast<uint32_t>(instances.size()),
        };

        _vertex_buffer_id = _ctx.create_vertex_buffer<Vert, Inst>(
            buffer_size, Topology::DIVISION_TOPOLOGY_TRIANGLES
        );
        {
            auto buffer_data =
                _ctx.borrow_vertex_buffer_data<Vert, Inst>(_vertex_buffer_id);

            std::copy(
                std::begin(verts), std::end(verts), buffer_data.per_vertex_data().begin()
            );

            std::copy(
                std::begin(instances),
                std::end(instances),
                buffer_data.per_instance_data().begin()
            );

            std::copy(
                std::begin(indices), std::end(indices), buffer_data.index_data().begin()
            );
        }

        _white_texture = DivisionIdWithBinding {
            .id = _ctx.create_texture({ 1, 1 }, DIVISION_TEXTURE_FORMAT_RGBA32Uint),
            .shader_location = 0,
        };

        const uint32_t RGBA32_WHITE_PIXEL = 0xFF'FF'FF'FF;
        _ctx.set_texture_data(
            _white_texture.id,
            reinterpret_cast<const uint8_t*>(&RGBA32_WHITE_PIXEL) // NOLINT
        );

        auto render_pass_desc_id =
            _ctx.render_pass_descriptor_builder()
                .shader(_shader_id)
                .vertex_buffer(_vertex_buffer_id)
                .build();

        _render_pass =
            RenderPassInstanceBuilder { render_pass_desc_id }
                .vertices(verts.size())
                .indices(indices.size())
                .instances(instances.size())
                .uniform_vertex_buffers({ &_screen_uniform, 1 })
                .uniform_fragment_buffers({ &_screen_uniform, 1 })
                .fragment_textures({ &_white_texture, 1 })
                .build();
    }

    ~MyLifecycleManager() { std::cout << "Lifecycle manager was destroyed" << std::endl; }

    void draw()
    {
        _ctx.draw_render_passes({ &_render_pass, 1 }, { 1, 1, 1, 1 });
    }

    void error(int32_t errorCode, const char* errorMessage)
    {
        std::cerr << "Error code: " << errorCode << ". Error message: " << errorMessage
                  << std::endl;
    }

private:
    DivisionId _shader_id;
    DivisionId _vertex_buffer_id;
    DivisionIdWithBinding _screen_uniform {};
    DivisionIdWithBinding _white_texture {};
    DivisionRenderPassInstance _render_pass {};
    Context _ctx;
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
    const size_t SCREEN_SIZE = 512;
    division_engine::core::CoreRunner coreRunner {
        "Core example",
        { SCREEN_SIZE, SCREEN_SIZE },
    };

    coreRunner.run(MyLifecycleManagerBuilder {});
}