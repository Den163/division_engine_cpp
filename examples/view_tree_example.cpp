#include "division_engine/canvas/border_radius.hpp"
#include "division_engine/canvas/padding.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/canvas/view_tree/decorated_box_view.hpp"
#include "division_engine/canvas/view_tree/list_view.hpp"
#include "division_engine/canvas/view_tree/padding_view.hpp"
#include "division_engine/canvas/view_tree/stack_view.hpp"
#include "division_engine/canvas/view_tree/text_view.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"
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

struct MyUIBuilder
{
    auto build_ui(State& state)
    {
        return make_horizontal_list(
            std::tuple {
                DecoratedBoxView { .background_color = color::RED },
                PaddingView {
                    DecoratedBoxView {
                        .background_color = color::BLUE,
                        .border_radius = BorderRadius::all(10),
                    },
                }
                    .with_padding(Padding::all(10)),
                DecoratedBoxView { .background_color = color::GREEN },
                StackView {
                    std::tuple {
                        DecoratedBoxView { .background_color = color::RED },
                        TextView { .text = u"Hey world", .color = color::BLACK },
                    }
                },
                DecoratedBoxView { .background_color = color::BLUE },
                DecoratedBoxView { .background_color = color::PURPLE },
            }
        );
    }
};

class MyLifecycleManager
{
public:
    using root_view =
        std::invoke_result<decltype(&MyUIBuilder::build_ui), MyUIBuilder, State&>::type;

    MyLifecycleManager(DivisionContext* ctx_ptr)
      : _state(State { ctx_ptr, color::WHITE })
      , _ui_builder(MyUIBuilder())
      , _root_view(_ui_builder.build_ui(_state))
      , _root_view_render(root_view::renderer_type::create(_state, _render_manager, _root_view)
        )
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
    MyUIBuilder _ui_builder;
    root_view _root_view;
    root_view::renderer_type _root_view_render;
};

struct MyBuilder
{
    using manager_type = MyLifecycleManager;

    manager_type* build(DivisionContext* ctx) { return new MyLifecycleManager { ctx }; }
};

int main(int argc, char** argv)
{
    CoreRunner core_runner { "View tree example", glm::vec2 { 512, 512 } }; // NOLINT
    core_runner.run(MyBuilder {});
}