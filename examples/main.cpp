#include "division_engine/core/context_helper.hpp"
#include "division_engine/core/types.hpp"
#include "division_engine_core/context.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/vertex_buffer.h"
#include "glm/fwd.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <iostream>

#include <division_engine/core/core_runner.hpp>
#include <division_engine/core/vertex_data.hpp>
#include <filesystem>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

using namespace division_engine::core;

struct MyVertexData
{
    glm::vec2 vertex_position;
    glm::vec2 uv;

    static constexpr DivisionVertexAttributeSettings vertex_attributes[] = {
        DivisionVertexAttributeSettings {
            .type = DivisionShaderVariableType::DIVISION_FVEC2,
            .location = 0,
        },
        DivisionVertexAttributeSettings {
            .type = DivisionShaderVariableType::DIVISION_FVEC2,
            .location = 1,
        },
    };
};

struct MyInstanceData
{
    static constexpr DivisionVertexAttributeSettings vertex_attributes[] = {
        DivisionVertexAttributeSettings {
            .type = DivisionShaderVariableType::DIVISION_FVEC2,
            .location = 0,
        },
        DivisionVertexAttributeSettings {
            .type = DivisionShaderVariableType::DIVISION_FVEC2,
            .location = 1,
        },
    };
};

struct MyLifecycleManager
{
    MyLifecycleManager(DivisionContext* context)
    {
        ContextHelper helper { context };
        _shader_id = helper.create_bundled_shader(
            std::filesystem::path { "resources" } / "shaders" / "canvas" / "rect");

        VertexBufferSize buffer_size { .vertex_count = 2,
                                       .index_count = 0,

                                       .instance_count = 2 };

        _vertex_buffer_id = helper.create_vertex_buffer<MyVertexData, MyInstanceData>(
            buffer_size, Topology::DIVISION_TOPOLOGY_TRIANGLES);
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