#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"

#include <filesystem>
#include <type_traits>

using namespace division_engine;
using namespace core;
using namespace canvas;

using std::filesystem::path;

const path FONT_PATH = path { "resources" } / "fonts" / "Roboto-Medium.ttf";

class MyLifecycleManager
{
public:
    MyLifecycleManager(DivisionContext* ctx_ptr)
      : _state(State { ctx_ptr })
    {
        _render_manager.register_renderer<RectDrawer>(_state);
        _render_manager.register_renderer<TextDrawer>(_state, FONT_PATH);
        _state.clear_color = color::WHITE;
    }

    void draw()
    {
        _state.update();
        _render_manager.update(_state);

        _state.render_queue.draw(_state.context.get_ptr(), _state.clear_color);
    }

    void error(int error_code, const char* error_message) {}

private:
    State _state;
    RenderManager _render_manager;
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