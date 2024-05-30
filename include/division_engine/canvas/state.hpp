#pragma once

#include "division_engine/color.hpp"
#include "division_engine/core/context.hpp"
#include "glm/ext/vector_float2.hpp"
#include "render_queue.hpp"

#include <chrono>
#include <ctime>
#include <division_engine_core/types/id.h>

#include <flecs.h>
#include <glm/vec4.hpp>

namespace division_engine::canvas
{

struct State
{
public:
    flecs::world world;
    glm::vec4 clear_color;

    core::Context context;
    DivisionId screen_size_uniform_id;
    DivisionId white_texture_id;
    RenderQueue render_queue;

private:
    glm::vec2 _prev_screen_size;
    size_t _frame_count;
    bool _screen_size_changed;

public:
    State(State&) = delete;
    State operator=(State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;
    ~State() = default;

    explicit State(DivisionContext* ctx_ptr, const glm::vec4& clear_color = color::BLACK)
      : clear_color(clear_color)
      , context(ctx_ptr)
      , screen_size_uniform_id(context.create_uniform<glm::vec2>())
      , white_texture_id(context.create_texture(
            { 1, 1 },
            DivisionTextureFormat::DIVISION_TEXTURE_FORMAT_RGBA32Uint
        ))
      , _prev_screen_size(glm::vec2 { 0 })
      , _frame_count(0)
      , _screen_size_changed(true)
    {
        const uint32_t RGBA32_WHITE_PIXEL = 0xFF'FF'FF'FF;
        context.set_texture_data(
            white_texture_id,
            reinterpret_cast<const uint8_t*>(&RGBA32_WHITE_PIXEL) // NOLINT
        );
    }

    void update()
    {
        const auto screen_size = context.get_screen_size();
        auto screen_uniform_data =
            context.get_uniform_data<glm::vec2>(screen_size_uniform_id);
        *screen_uniform_data.data_ptr = screen_size;

        _screen_size_changed = screen_size != _prev_screen_size;
        _prev_screen_size = screen_size;

        _frame_count++;
    }

    bool screen_size_changed() const { return _screen_size_changed; }
    size_t frame_count() const { return _frame_count; } 
};
}