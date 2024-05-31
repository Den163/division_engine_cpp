#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/canvas/view_tree/any_view.hpp"
#include "division_engine/canvas/view_tree/decorated_box.hpp"
#include "division_engine/canvas/view_tree/list.hpp"
#include "division_engine/canvas/view_tree/padding.hpp"
#include "division_engine/canvas/view_tree/sized_box.hpp"
#include "division_engine/canvas/view_tree/stack.hpp"
#include "division_engine/canvas/view_tree/text.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "division_engine/canvas/view_tree/view_builder.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
#include "division_engine_core/context.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace division_engine;
using namespace core;
using namespace canvas;
using namespace view_tree;

using std::filesystem::path;

const path FONT_PATH = path { "resources" } / "fonts" / "Roboto-Regular.ttf";

template<typename T>
concept UIBuilder = requires(T t, State& state) {
                        {
                            t.build_ui(state)
                        } -> View;
                    };

struct MyUIBuilder
{
    using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

    time_point_t _prev_time = std::chrono::steady_clock::now();

    View auto build_ui(State& state)
    {
        const auto now = std::chrono::steady_clock::now();
        const auto delta_time = (now - _prev_time);
        const auto delta_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(delta_time).count();

        _prev_time = now;

        return HorizontalList {
            SizedBox {
                Size { 200, 100 },
                Stack {
                    DecoratedBox { .background_color = glm::vec4 { 0.5, 0.5, 0.5, 1 } },
                    Text {
                        .text = std::string { "UI build delta time: " } +
                                std::to_string(delta_ms) + " ms",
                        .color = color::BLACK,
                    },
                },
            },
        };
    }
};

template<UIBuilder T>
class MyLifecycleManager
{
public:
    using root_view_t = typename std::invoke_result_t<decltype(&T::build_ui), T, State&>;
    using root_view_renderer_t = typename root_view_t::Renderer;

    MyLifecycleManager(DivisionContext* ctx_ptr, T ui_builder)
      : _state(State { ctx_ptr, color::WHITE })
      , _ui_builder(ui_builder)
      , _root_view(std::make_unique<root_view_t>(_ui_builder.build_ui(_state)))
      , _root_view_render(std::make_unique<root_view_renderer_t>(
            _state,
            _render_manager,
            *_root_view.get()
        ))
    {
        _render_manager.register_renderer<RectDrawer>(_state);
        _render_manager.register_renderer<TextDrawer>(_state, FONT_PATH);
    }

    void draw()
    {
        _rebuild_ui = true;

        _state.update();
        _render_manager.update(_state);

        const auto screen_size = _state.context.get_screen_size();
        auto screen_rect = Rect::from_bottom_left(glm::vec2 { 0 }, screen_size);

        if (_rebuild_ui)
        {
            std::exchange(*_root_view, _ui_builder.build_ui(_state));
            _root_view_render->render(_state, _render_manager, screen_rect, *_root_view);
        }

        _state.render_queue.draw(_state.context.get_ptr(), _state.clear_color);
    }

    void error(int error_code, const char* error_message)
    {
        std::cerr << error_message << std::endl;
    }

private:
    State _state;
    RenderManager _render_manager;
    T _ui_builder;
    std::unique_ptr<root_view_t> _root_view;
    std::unique_ptr<root_view_renderer_t> _root_view_render;

    bool _rebuild_ui = true;
};

struct MyBuilder
{
    MyLifecycleManager<MyUIBuilder>* build(DivisionContext* ctx)
    {
        return new MyLifecycleManager { ctx, MyUIBuilder {} };
    }
};

int main(int argc, char** argv)
{
    CoreRunner core_runner { "View tree example", glm::vec2 { 512, 512 } }; // NOLINT
    core_runner.run(MyBuilder {});
}