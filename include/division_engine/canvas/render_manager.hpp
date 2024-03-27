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

        std::unique_ptr<Renderer> ptr = std::make_unique<TRenderer>(args...);
        _renderers.push_back(std::move(ptr));
    }

    template<typename... TComponents, typename... TBatchComponents>
    flecs::entity create_renderer(
        State& state,
        std::tuple<TComponents...> components,
        std::tuple<TBatchComponents...> batch_components = std::make_tuple()
    )
    {
        using namespace division_engine::utility::algorithm;
        using components::RenderBatch;
        using components::RenderOrder;

        auto renderer_id = std::type_index(typeid(components));
        if (!_batch.has_value() || _batch->first != renderer_id)
        {
            _batch = std::make_pair(
                renderer_id, state.world.entity().set(RenderBatch {}).id()
            );
        }

        auto entity = state.world.entity();
        tuple_foreach([&](auto& comp) { entity.set(comp); }, components);
        tuple_foreach(
            [&](auto& batch_comp) { entity.is_a(batch_comp); }, batch_components
        );
        
        entity.set(RenderOrder { _render_order++ });
        entity.is_a(_batch->second);

        return entity;
    }

    void update(State& state)
    {
        using namespace division_engine::utility::algorithm;

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