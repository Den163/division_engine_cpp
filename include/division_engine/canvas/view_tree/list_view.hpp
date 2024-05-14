#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size_variant.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/utility/algorithm.hpp"
#include "glm/ext/vector_float2.hpp"

#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace division_engine::canvas::view_tree
{
enum class Direction
{
    Horinzontal,
    Vertical
};

template<typename... TChildRenderer>
struct ListViewRender;

template<typename TView>
using view_renderer_t = typename TView::renderer;

template<Direction elements_direction, typename... TChildView>
struct ListView
{
    using renderer = ListViewRender<view_renderer_t<TChildView>...>;

    std::tuple<TChildView...> children;
};

template<typename... TChildView>
struct HorizontalListView : ListView<Direction::Horinzontal, TChildView...>
{
};

template<typename... TChildView>
HorizontalListView(std::tuple<TChildView...>) -> HorizontalListView<TChildView...>;

template<typename... TChildView>
struct VerticalListView : ListView<Direction::Vertical, TChildView...>
{
};

template<typename... TChildView>
VerticalListView(std::tuple<TChildView...>) -> VerticalListView<TChildView...>;

template<typename... TChildRenderer>
struct ListViewRender
{
    std::tuple<TChildRenderer...> children;

    template<Direction elements_direction, typename... TChildView>
    static ListViewRender<TChildRenderer...> create(
        State& state,
        RenderManager& render_manager,
        const ListView<elements_direction, TChildView...>& view
    )
    {
        return ListViewRender<TChildRenderer...> {
            .children = utility::algorithm::tuple_transform(
                [&](auto v)
                {
                    using view_type = decltype(v);

                    return view_type::renderer::create(state, render_manager, v);
                },
                view.children
            )
        };
    }

    template<Direction elements_direction, typename... TChildView>
    SizeVariant layout(
        const BoxConstraints& constraints,
        const ListView<elements_direction, TChildView...>& view
    )
    {
        return SizeVariant::fill();
    }

    template<Direction elements_direction, typename... TChildView>
    void render(
        State& state,
        RenderManager& render_manager,
        Rect& rect,
        const ListView<elements_direction, TChildView...>& view
    )
    {
        const auto rect_size = rect.size();

        glm::vec2 available_size = rect_size;
        glm::vec2 all_fixed_size = glm::vec2 { 0 };
        int fixed_size_count = 0;

        utility::algorithm::tuples_zip(
            [&](auto& child_renderer, auto& child_view)
            {
                BoxConstraints constraints {
                    .min_size = glm::vec2 { 0 },
                    .max_size = glm::vec2 { available_size.x, available_size.y },
                };

                const SizeVariant child_size =
                    child_renderer.layout(constraints, child_view);

                if (child_size.is_fixed())
                {
                    const auto fixed_size =
                        std::get<SizeVariant::Fixed>(child_size.variant);
                    fixed_size_count++;

                    if constexpr (elements_direction == Direction::Horinzontal)
                    {
                        available_size.x -= fixed_size.size.x;
                        all_fixed_size.x += fixed_size.size.x;
                    }
                    else if constexpr (elements_direction == Direction::Vertical)
                    {
                        available_size.y -= fixed_size.size.y;
                        all_fixed_size.y += fixed_size.size.y;
                    }
                }
            },
            children,
            view.children
        );

        const auto filled_size_count = sizeof...(TChildView) - fixed_size_count;

        const glm::vec2 filled_space = (rect_size - all_fixed_size);
        glm::vec2 size_per_filled = rect_size;
        if constexpr (elements_direction == Direction::Horinzontal)
        {
            size_per_filled.x = filled_space.x / filled_size_count;
        }
        else if constexpr (elements_direction == Direction::Vertical)
        {
            size_per_filled.y = filled_space.y / filled_size_count;
        }

        available_size = rect_size;

        glm::vec2 offset { 0 };

        utility::algorithm::tuples_zip(
            [&](auto& child_renderer, auto& child_view)
            {
                BoxConstraints constraints { .min_size = glm::vec2 { 0 },
                                             .max_size = available_size };
                const SizeVariant child_size =
                    child_renderer.layout(constraints, child_view);

                auto top_left =
                    glm::vec2 { rect.left() + offset.x, rect.top() - offset.y };

                if (child_size.is_filled())
                {
                    auto draw_rect = Rect::from_top_left( top_left, size_per_filled );
                    child_renderer.render(state, render_manager, draw_rect, child_view);

                    if constexpr (elements_direction == Direction::Horinzontal)
                    {
                        offset.x += size_per_filled.x;
                    }
                    else if constexpr (elements_direction == Direction::Vertical)
                    {
                        offset.y += size_per_filled.y;
                    }
                }
                else if (child_size.is_fixed())
                {
                    auto fixed_size = std::get<SizeVariant::Fixed>(child_size.variant);
                    auto draw_rect = Rect::from_top_left(top_left, fixed_size.size);
                    child_renderer.render(state, render_manager, draw_rect, child_view);

                    if constexpr (elements_direction == Direction::Horinzontal)
                    {
                        offset.x += draw_rect.size().x;
                    }
                    else if constexpr (elements_direction == Direction::Vertical)
                    {
                        offset.y += draw_rect.size().y;
                    }
                }
            },
            children,
            view.children
        );
    }
};
}