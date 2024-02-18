#pragma once

#include "components/render_batch.hpp"
#include "components/render_order.hpp"
#include "renderer.hpp"
#include "state.hpp"

#include "division_engine/utility/algorithm.hpp"

#include <flecs.h>

#include <optional>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace division_engine::canvas
{

template<Renderer... TArgs>
class RenderManager
{
public:
    RenderManager(TArgs&&... renderers)
      : _renderers(std::forward_as_tuple(renderers...))
      , _batch(std::nullopt)
      , _render_order(0)
    {
    }

    template<typename... TComponents, typename... TBatchComponents>
    void create_renderer(
        State& state,
        const std::tuple<TComponents...>& components,
        const std::tuple<TBatchComponents...>& batch_components
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
        tuple_foreach([&](const auto& comp) { entity.set(comp); }, components);
        tuple_foreach(
            [&](const auto& batch_comp) { entity.is_a(batch_comp); }, batch_components
        );

        entity.set(RenderOrder { _render_order++ });
        entity.is_a(_batch->second);
    }

    void update(State& state)
    {
        using namespace division_engine::utility::algorithm;

        tuple_foreach([&](const auto& renderer) { renderer.draw(state); }, _renderers);
    }

private:
    std::tuple<TArgs...> _renderers;
    std::optional<std::pair<std::type_index, flecs::entity_t>> _batch;
    uint32_t _render_order;
};

}