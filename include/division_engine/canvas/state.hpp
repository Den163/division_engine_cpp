#pragma once

#include "division_engine/core/context.hpp"
#include "division_engine/color.hpp"
#include "render_queue.hpp"

#include <division_engine_core/types/id.h>

#include <flecs.h>
#include <glm/vec4.hpp>

namespace division_engine::canvas
{

struct State
{
    flecs::world world;
    glm::vec4 clear_color;

    core::Context context;
    DivisionId screen_size_uniform_id;
    DivisionId white_texture_id;
    RenderQueue render_queue;

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
    {
        const uint32_t RGBA32_WHITE_PIXEL = 0xFFFFFFFF;
        context.set_texture_data(
            white_texture_id, 
            reinterpret_cast<const uint8_t*>(&RGBA32_WHITE_PIXEL) // NOLINT
        );

        update();
    }

    void update()
    {
        auto screen_uniform_data =
            context.get_uniform_data<glm::vec2>(screen_size_uniform_id);
        *screen_uniform_data.data_ptr = context.get_screen_size();
    }
};
}