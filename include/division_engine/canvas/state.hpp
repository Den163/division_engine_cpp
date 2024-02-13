#pragma once

#include "division_engine/core/context_helper.hpp"
#include "division_engine/core/types.hpp"
#include "render_queue.hpp"

#include <division_engine_core/context.h>
#include <division_engine_core/renderer.h>
#include <flecs.h>
#include <glm/vec4.hpp>

namespace division_engine::canvas
{
using DivisionId = core::DivisionId;

struct State
{
    flecs::world world;
    glm::vec4 clear_color;

    core::ContextHelper context_helper;
    DivisionId screen_size_uniform_id;
    DivisionId white_texture_id;
    RenderQueue render_queue;

    State(State&) = delete;
    State operator=(State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;
    ~State() = default;

    State(DivisionContext* context)
      : clear_color({ 0, 0, 0, 1 })
      , context_helper(context)
      , screen_size_uniform_id(context_helper.create_uniform<glm::vec2>())
      , white_texture_id(context_helper.create_texture(
            { 1, 1 },
            DivisionTextureFormat::DIVISION_TEXTURE_FORMAT_RGBA32Uint
        ))
    {
        const auto tex_data = std::array<uint8_t, 4> { 0xFF };
        context_helper.set_texture_data(white_texture_id, tex_data.data());

        update();
    }

    void update()
    {
        auto screen_uniform_data =
            context_helper.get_uniform_data<glm::vec2>(screen_size_uniform_id);
        *screen_uniform_data.data_ptr = context_helper.get_frame_buffer_size();
    }
};
}