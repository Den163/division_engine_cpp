#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/components.hpp"
#include "division_engine/canvas/components/render_batch.hpp"
#include "division_engine/canvas/components/render_bounds.hpp"
#include "division_engine/canvas/components/render_order.hpp"
#include "division_engine/canvas/components/render_texture.hpp"
#include "division_engine/canvas/components/renderable_rect.hpp"
#include "division_engine/canvas/components/renderable_text.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
#include "division_engine/core/font_texture.hpp"
#include "division_engine/core/lifecycle_manager.hpp"

#include <flecs.h>
#include <glm/gtc/random.hpp>
#include <glm/vec2.hpp>

#include <array>
#include <filesystem>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <tuple>
#include <utility>

using namespace division_engine;
using namespace division_engine::canvas;
using namespace division_engine::canvas::components;
using namespace division_engine::core;

const size_t RECT_COUNT = 100'000;
const size_t RECT_SIZE = 8;
const size_t TEXT_RECT_SIZE = 256;

const auto FONT_SIZE = 20;
const auto FONT_PATH =
    std::filesystem::path { "resources" } / "fonts" / "Roboto-Medium.ttf";

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
      , _query(_state.world.query<RenderBounds, RenderableRect, Velocity>())
    {
        _renderer_manager.register_renderer<RectDrawer>(_state);
        _renderer_manager.register_renderer<TextDrawer>(_state, FONT_PATH);

        const auto with_white_tex =
            _state.world.entity().set(RenderTexture { _state.white_texture_id });

        _state.clear_color = color::WHITE;

        const auto screen_size = _state.context.get_screen_size();

        for (int i = 0; i < RECT_COUNT; i++)
        {
            _renderer_manager
                .create_renderer(
                    _state,
                    std::make_tuple(
                        RenderableRect {
                            .color = glm::linearRand(color::WHITE, color::BLACK),
                        },
                        RenderBounds {
                            Rect::from_center(
                                glm::linearRand(glm::vec2 { 0 }, screen_size),
                                glm::vec2 { RECT_SIZE }
                            ),
                        }
                    ),
                    with_white_tex.id()
                )
                .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) });
        }

        _renderer_manager
            .create_renderer(
                _state,
                std::make_tuple(
                    RenderableText {
                        .text = "Hello text drawer",
                        .color = color::PURPLE,
                        .font_size = FONT_SIZE,
                    },
                    RenderBounds { Rect::from_center(
                        glm::vec2 { 256 }, glm::vec2 { TEXT_RECT_SIZE }
                    ) }
                )
            )
            .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) });

        _renderer_manager
            .create_renderer(
                _state,
                std::make_tuple(
                    RenderableRect {
                        .color = color::RED,
                        .border_radius = BorderRadius::all(10),
                    },
                    RenderBounds {
                        Rect::from_center(
                            glm::linearRand(glm::vec2 { 0 }, screen_size),
                            glm::vec2 { 256 }
                        ),
                    }
                ),
                with_white_tex.id()
            )
            .set(Velocity { glm::linearRand(glm::vec2 { -1 }, glm::vec2 { 1 }) });
    }

    void draw()
    {
        _state.update();
        _renderer_manager.update(_state);

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
    flecs::query<RenderBounds, RenderableRect, Velocity> _query;
    RenderManager _renderer_manager;
};

struct MyManagerBuilder
{
    using manager_type = MyManager;

    MyManager* build(DivisionContext* context) { return new MyManager { context }; }
};

int main(int argc, char** argv)
{
    const size_t WINDOW_SIZE = 512;
    CoreRunner { "Canvas example", { WINDOW_SIZE, WINDOW_SIZE } }.run(MyManagerBuilder {});
}