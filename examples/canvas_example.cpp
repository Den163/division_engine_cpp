#include "division_engine/core/context_helper.hpp"
#include "division_engine/core/lifecycle_manager.hpp"
#include "division_engine_core/context.h"
#include "glm/fwd.hpp"

#include "division_engine/canvas/components.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/core/core_runner.hpp"
#include "glm/vec2.hpp"

#include <functional>
#include <iostream>

using namespace division_engine::canvas;
using namespace division_engine::canvas::components;
using namespace division_engine::core;

struct MyManager
{
    MyManager(DivisionContext* context)
      : state(context)
      , rect_drawer(state)
    {
        auto with_white_tex =
            state.world.entity().set(RenderTexture { state.white_texture_id });

        state.world.entity()
            .set(RectInstance {
                .size = { 100, 100 },
                .position = glm::vec2 { 0 },
                .color = { 0, 1, 0, 1 },
                .trbl_border_radius = glm::vec4 { 0 },
            })
            .is_a(with_white_tex);

        state.world.entity()
            .set(RectInstance {
                .size = { 100, 100 },
                .position = { 256, 256 },
                .color = { 0, 1, 0, 1 },
                .trbl_border_radius = glm::vec4 { 10 },
            })
            .is_a(with_white_tex);
    }

    void draw(DivisionContext* context)
    {
        state.update();
        rect_drawer.update(state);
    }

    void cleanup(DivisionContext* context) {}

    void error(DivisionContext* context, int error_code, const char* error_message)
    {
        std::cout << "Error code: " << error_code << ". Message: " << error_message
                  << std::endl;
    }

    State state;
    RectDrawer rect_drawer;
};

struct MyManagerBuilder
{
    using manager_type = MyManager;

    MyManager* build(DivisionContext* context) { return new MyManager { context }; }
};

int main(int argc, char** argv)
{
    MyManagerBuilder manager;
    CoreRunner { "Canvas example", { 512, 512 } }.run(manager);
}