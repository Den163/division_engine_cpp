#pragma once

#include "division_engine/core/context_helper.hpp"
#include "glm/ext/vector_float2.hpp"
#include <division_engine/core/types.hpp>
#include <flecs.h>

#include "division_engine_core/context.h"
#include "division_engine_core/renderer.h"

namespace division_engine::canvas
{
using DivisionId = core::DivisionId;

struct State
{
    flecs::world world;
    core::ContextHelper context_helper;

    DivisionId screen_size_uniform_id;
    DivisionId white_texture_id;

    State(DivisionContext* context)
      : context_helper(context)
    {
        screen_size_uniform_id = context_helper.create_uniform<glm::vec2>();
        white_texture_id = context_helper.create_texture(
            { 1, 1 }, DivisionTextureFormat::DIVISION_TEXTURE_FORMAT_RGBA32Uint
        );
        
        uint8_t tex_data[] = { 0xFF, 0xFF, 0xFF, 0xFF };
        context_helper.set_texture_data(white_texture_id, tex_data);

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