#include "division_engine/canvas/components/rect_instance.hpp"
#include "division_engine/core/context_helper.hpp"
#include "division_engine/core/lifecycle_manager.hpp"
#include "division_engine_core/context.h"
#include "glm/fwd.hpp"

#include "division_engine/canvas/components.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/core/core_runner.hpp"
#include "glm/gtc/random.hpp"
#include "glm/vec2.hpp"

#include <functional>
#include <iostream>

const size_t RECT_COUNT = 1'000'000;

using namespace division_engine;
using namespace division_engine::canvas;
using namespace division_engine::canvas::components;
using namespace division_engine::core;

struct Velocity
{
    glm::vec2 value;
};

struct MyManager
{
    MyManager() = delete;
    MyManager(MyManager&&) = delete;
    MyManager& operator=(MyManager&&) = delete;
    MyManager(MyManager&) = delete;
    MyManager operator=(MyManager&) = delete;
    ~MyManager() = default;

    MyManager(DivisionContext* context)
      : state(context)
      , rect_drawer(state)
      , _query(state.world.query<RectInstance, Velocity>())
    {
        auto with_white_tex =
            state.world.entity().set(RenderTexture { state.white_texture_id });

        state.clear_color = color::WHITE;

        const size_t RECT_SIZE = 10;
        const auto screen_size = state.context_helper.get_screen_size();

        for (int i = 0; i < RECT_COUNT; i++)
        {
            state.world.entity()
                .set(RectInstance {
                    .size { RECT_SIZE },
                    .position = glm::linearRand(glm::vec2 { 0 }, screen_size),
                    .color = glm::linearRand(color::BLACK, color::WHITE),
                    .trbl_border_radius { 0 },
                })
                .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) })
                .is_a(with_white_tex);
        }
    }

    void draw(DivisionContext* context)
    {
        state.update();
        rect_drawer.update(state);

        update_rects();
        
        state.render_queue.draw(context, state.clear_color);
    }

    void update_rects()
    {
        const auto screen_size = state.context_helper.get_screen_size();

        _query.each(
            [screen_size](RectInstance& rect, Velocity& vel)
            {
                auto& dir = vel.value;
                auto next_pos = rect.position + dir;
                if (next_pos.x > screen_size.x)
                {
                    next_pos.x = screen_size.x;
                    dir.x = -dir.x;
                }
                else if (next_pos.x < 0)
                {
                    next_pos.x = 0;
                    dir.x = -dir.x;
                }

                if (next_pos.y > screen_size.y)
                {
                    next_pos.y = screen_size.y;
                    dir.y = -dir.y;
                }
                else if (next_pos.y < 0)
                {
                    next_pos.y = 0;
                    dir.y = -dir.y;
                }

                rect.position = next_pos;
            }
        );
    }

    void cleanup(DivisionContext* context) {}

    void error(DivisionContext* context, int error_code, const char* error_message)
    {
        std::cout << "Error code: " << error_code << ". Message: " << error_message
                  << std::endl;
    }

    State state;
    RectDrawer rect_drawer;
    flecs::query<RectInstance, Velocity> _query;
};

struct MyManagerBuilder
{
    using manager_type = MyManager;

    MyManager* build(DivisionContext* context) { return new MyManager { context }; }
};

int main(int argc, char** argv)
{
    const size_t WINDOW_SIZE = 512;
    MyManagerBuilder manager;
    CoreRunner { "Canvas example", { WINDOW_SIZE, WINDOW_SIZE } }.run(manager);
}