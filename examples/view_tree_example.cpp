#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/padding.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/canvas/view_tree/any_view.hpp"
#include "division_engine/canvas/view_tree/decorated_box_view.hpp"
#include "division_engine/canvas/view_tree/list_view.hpp"
#include "division_engine/canvas/view_tree/padding_view.hpp"
#include "division_engine/canvas/view_tree/size_view.hpp"
#include "division_engine/canvas/view_tree/stack_view.hpp"
#include "division_engine/canvas/view_tree/text_view.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
#include "division_engine_core/context.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <filesystem>
#include <iostream>
#include <tuple>
#include <type_traits>

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
    View auto build_ui(State& state)
    {
        return HorizontalListView {
            DecoratedBoxView { .background_color = color::RED },
            SizeView {
                StackView {
                    TextView { .text = u"Hello world", .color = color::RED },
                    DecoratedBoxView { .background_color = color::YELLOW },
                },
            }
                .with_size(glm::vec2 { 200, 100 }),
            PaddingView {
                DecoratedBoxView {
                    .background_color = color::BLUE,
                    .border_radius = BorderRadius::all(10),
                },
            }
                .with_padding(Padding::all(10)),
            DecoratedBoxView { .background_color = color::GREEN },
            VerticalListView {
                DecoratedBoxView { .background_color = color::AQUA },
                StackView {
                    DecoratedBoxView { .background_color = color::RED },
                    TextView {
                        .text = u"Hey world",
                        .color = color::BLACK,
                    },
                },
            },
            DecoratedBoxView { .background_color = color::BLUE },
            DecoratedBoxView { .background_color = color::PURPLE },
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
      , _root_view(_ui_builder.build_ui(_state))
      , _root_view_render(root_view_renderer_t { _state, _render_manager, _root_view })
    {
        _render_manager.register_renderer<RectDrawer>(_state);
        _render_manager.register_renderer<TextDrawer>(_state, FONT_PATH);
    }

    void draw()
    {
        _state.update();
        _render_manager.update(_state);

        auto screen_size = _state.context.get_screen_size();
        auto screen_rect = Rect::from_bottom_left(glm::vec2 { 0 }, screen_size);

        _root_view_render.render(_state, _render_manager, screen_rect, _root_view);
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
    root_view_t _root_view;
    root_view_renderer_t _root_view_render;
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