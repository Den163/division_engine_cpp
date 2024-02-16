#include "division_engine/canvas/components.hpp"
#include "division_engine/canvas/components/rect_instance.hpp"
#include "division_engine/canvas/components/render_order.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
#include "division_engine/core/font_texture.hpp"
#include "division_engine/core/lifecycle_manager.hpp"

#include "glm/gtc/random.hpp"
#include "glm/vec2.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>

const size_t RECT_COUNT = 2;

const auto FONT_SIZE = 64;
const auto FONT_PATH =
    std::filesystem::path { "resources" } / "fonts" / "Roboto-Medium.ttf";

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

    MyManager(DivisionContext* context_ptr)
      : _state(context_ptr)
      , _rect_drawer(_state)
      , _font_texture( FontTexture { _state.context, FONT_PATH, FONT_SIZE } )
      , _query(_state.world.query<RectInstance, Velocity>())
    {
        const auto with_white_tex =
            _state.world.entity().set(RenderTexture { _font_texture.texture_id() });
        const std::basic_string<char32_t> input_string  { U"Привет!" };
        for (char32_t ch : input_string)
        {
            _font_texture.reserve_character(ch);
        }

        _font_texture.upload_texture();

        _state.clear_color = color::WHITE;

        const size_t RECT_SIZE = 256;
        const auto screen_size = _state.context.get_screen_size();

        for (int i = 0; i < RECT_COUNT; i++)
        {
            _state.world.entity()
                .set(RectInstance {
                    .size { RECT_SIZE },
                    .position = glm::linearRand(glm::vec2 { 0 }, screen_size),
                    .color = glm::vec4 { 1 },
                    .trbl_border_radius { 0 },
                })
                .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) })
                .set(RenderOrder { static_cast<uint32_t>(i) })
                .is_a(with_white_tex);
        }
    }

    void draw()
    {
        _state.update();
        _rect_drawer.update(_state);

        update_rects();

        _state.render_queue.draw(_state.context.get_ptr(), _state.clear_color);
    }

    void update_rects()
    {
        const auto screen_size = _state.context.get_screen_size();

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

    void error(int error_code, const char* error_message)
    {
        std::cout << "Error code: " << error_code << ". Message: " << error_message
                  << std::endl;
    }

    State _state;
    RectDrawer _rect_drawer;
    FontTexture _font_texture;
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