#include "division_engine/canvas/rect_drawer.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/text_drawer.hpp"
#include "division_engine/core/context.hpp"
#include "division_engine/core/core_runner.hpp"

#include <filesystem>

using namespace division_engine::core;
using namespace division_engine::canvas;
using std::filesystem::path;
// using namespace division_engine::canvas::components;

using MyRendererManager = RenderManager<RectDrawer, TextDrawer>;

const path FONT_PATH = path { "resources" } / "fonts" / "Roboto-Medium.ttf";

class MyLifecycleManager
{
public:
    MyLifecycleManager(DivisionContext* ctx_ptr)
      : _state(State { ctx_ptr })
      , _render_manager(MyRendererManager {
            RectDrawer { _state },
            TextDrawer { _state, FONT_PATH },
        })
    {
    }

    void draw() {}

    void error(int error_code, const char* error_message) {}

private:
    State _state;
    MyRendererManager _render_manager;
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