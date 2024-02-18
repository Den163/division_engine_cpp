#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/components.hpp"
#include "division_engine/canvas/components/render_bounds.hpp"
#include "division_engine/canvas/components/render_order.hpp"
#include "division_engine/canvas/components/renderable_rect.hpp"
#include "division_engine/canvas/components/renderable_text.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
#include "division_engine/core/font_texture.hpp"
#include "division_engine/core/lifecycle_manager.hpp"

#include "glm/ext/vector_float2.hpp"
#include "glm/gtc/random.hpp"
#include "glm/vec2.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <locale>
#include <string>

const size_t RECT_COUNT = 2;
const size_t RECT_SIZE = 128;

const auto FONT_SIZE = 20;
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
      , _text_drawer(_state.context, _state, FONT_PATH)
      , _query(_state.world.query<RenderBounds, RenderableRect, Velocity>())
    {
        const auto with_white_tex =
            _state.world.entity().set(RenderTexture { _state.white_texture_id });

        const std::u16string input_string { u"Привет" };

        _state.clear_color = color::WHITE;

        const auto screen_size = _state.context.get_screen_size();

        _state.world.entity()
            .set(RenderableText {
                .color = color::BLACK,
                .text = u"Hello text drawer",
                .font_size = FONT_SIZE
            })
            .set(RenderBounds {
                Rect::from_center(glm::vec2 { 256,256 }, glm::vec2 { 256, 256 })
            })
            .set(RenderOrder { 10 });

        for (int i = 0; i < RECT_COUNT; i++)
        {
            _state.world.entity()
                .set(RenderableRect {
                    .color = glm::linearRand(color::BLACK, color::WHITE),
                    .border_radius = BorderRadius::all(0),
                })
                .set(RenderBounds { Rect::from_center(
                    glm::linearRand(glm::vec2 { 0 }, screen_size), glm::vec2 { RECT_SIZE }
                ) })
                .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) })
                .set(RenderOrder { static_cast<uint32_t>(i) })
                .is_a(with_white_tex);
        }
    }

    void draw()
    {
        _state.update();
        _rect_drawer.update(_state);
        _text_drawer.update(_state);

        update_rects();

        _state.render_queue.draw(_state.context.get_ptr(), _state.clear_color);
    }

    void update_rects()
    {
        const auto screen_size = _state.context.get_screen_size();

        _query.each(
            [screen_size](RenderBounds& bounds, RenderableRect& rect, Velocity& vel)
            {
                auto& rect_bounds = bounds.value;
                auto& dir = vel.value;

                rect_bounds.center += dir;
                if (rect_bounds.right() > screen_size.x)
                {
                    rect_bounds.set_right(screen_size.x);
                    dir.x = -dir.x;
                }
                else if (rect_bounds.left() < 0)
                {
                    rect_bounds.set_left(0);
                    dir.x = -dir.x;
                }

                if (rect_bounds.top() > screen_size.y)
                {
                    rect_bounds.set_top(screen_size.y);
                    dir.y = -dir.y;
                }
                else if (rect_bounds.bottom() < 0)
                {
                    rect_bounds.set_bottom(0);
                    dir.y = -dir.y;
                }
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
    TextDrawer _text_drawer;
    flecs::query<RenderBounds, RenderableRect, Velocity> _query;
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