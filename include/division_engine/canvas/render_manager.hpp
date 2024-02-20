#pragma once

#include "components/render_batch.hpp"
#include "components/render_order.hpp"
#include "renderer.hpp"
#include "state.hpp"

#include "division_engine/utility/algorithm.hpp"

#include <array>
#include <flecs.h>

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

template<Renderer... TArgs>
class RenderManager
{
public:
    RenderManager(TArgs&&... renderers)
      : _renderers(std::tuple<TArgs...>(std::forward<TArgs>(renderers)...))
      , _batch(std::nullopt)
      , _render_order(0)
    {
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

        tuple_foreach([&](auto& renderer) { renderer.update(state); }, _renderers);
    }

private:
    std::tuple<TArgs...> _renderers;
    std::optional<std::pair<std::type_index, flecs::entity_t>> _batch;
    uint32_t _render_order;

    template<typename... TComponents>
    static constexpr bool is_registered_renderable()
    {
        using namespace division_engine::utility::algorithm;

        bool has = false;

        tuple_foreach(
            [&](const auto& renderer)
            {
                if (is_registered_renderable<decltype(renderer), TComponents...>())
                {
                    has = true;
                }
            },
            std::declval<std::tuple<TComponents...>>()
        );

        return has;
    }

    template<Renderer TR, typename... TComponents>
    static constexpr bool is_registered_renderable()
    {
        return std::is_same<typename TR::renderable_type, std::tuple<TComponents...>>();
    }
};

}