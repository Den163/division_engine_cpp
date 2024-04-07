#pragma once

#include "components/render_batch.hpp"
#include "components/render_order.hpp"
#include "renderer.hpp"
#include "state.hpp"

#include "division_engine/utility/algorithm.hpp"

#include <array>
#include <flecs.h>

#include <memory>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace division_engine::canvas
{

class RenderManager
{
public:
    template<typename... TRenderers>
    RenderManager()
      : _renderers()
      , _batch(std::nullopt)
      , _render_order(0)
    {
    }

    template<typename TRenderer, typename... TArgs>
    void register_renderer(TArgs&... args)
    {
        static_assert(std::is_base_of<Renderer, TRenderer>());

        auto ptr = std::make_unique<TRenderer>(args...);
        _renderers.push_back(std::move(ptr));
    }

    template<typename... TComponents, size_t BATCH_COMPONENTS_COUNT = 0>
    flecs::entity create_renderer(
        State& state,
        std::tuple<TComponents...> components,
        std::array<flecs::entity_t, BATCH_COMPONENTS_COUNT> batch_components =
            std::array<flecs::entity_t, 0> {}
    )
    {
        using components::RenderBatch;
        using components::RenderOrder;
        using division_engine::utility::algorithm::tuple_foreach;

        auto renderer_id = std::type_index(typeid(components));
        if (!_batch.has_value() || _batch->first != renderer_id)
        {
            _batch = std::make_pair(
                renderer_id, state.world.entity().set(RenderBatch {}).id()
            );
        }

        auto new_entity = state.world.entity();
        tuple_foreach([&](auto& comp) { new_entity.set(comp); }, components);

        for (auto& batch_comp : batch_components)
        {
            new_entity.is_a(batch_comp);
        }

        new_entity.set(RenderOrder { _render_order++ });
        new_entity.is_a(_batch->second);

        return new_entity;
    }

    void update(State& state)
    {
        for (auto& rend : _renderers)
        {
            rend->fill_render_queue(state);
        }
    }

private:
    std::vector<std::unique_ptr<Renderer>> _renderers;
    std::optional<std::pair<std::type_index, flecs::entity_t>> _batch;
    uint32_t _render_order;
};

}