#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size_variant.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/utility/algorithm.hpp"
#include "glm/ext/vector_float2.hpp"
#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace division_engine::canvas::view_tree
{
template<typename... TChildRenderer>
struct ColumnViewRender;

template<typename TView>
using view_renderer_t = typename TView::renderer;

template<typename... TChildView>
struct ColumnView
{
    using renderer = ColumnViewRender<view_renderer_t<TChildView>...>;

    std::tuple<TChildView...> children;
};

template<typename... TChildView>
ColumnView(std::tuple<TChildView...> children) -> ColumnView<TChildView...>;

template<typename... TChildRenderer>
struct ColumnViewRender
{
    std::tuple<TChildRenderer...> children;

    template<typename... TChildView>
    static ColumnViewRender<TChildRenderer...> create(
        State& state,
        RenderManager& render_manager,
        const ColumnView<TChildView...>& view
    )
    {
        return ColumnViewRender<TChildRenderer...> {
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

    template<typename... TChildView>
    SizeVariant
    layout(const BoxConstraints& constraints, const ColumnView<TChildView...>& view)
    {
        return SizeVariant::fill();
    }

    template<typename... TChildView>
    void render(
        State& state,
        RenderManager& render_manager,
        Rect& rect,
        const ColumnView<TChildView...>& view
    )
    {
        const auto rect_size = rect.size();

        double available_height = rect.size().y;
        double fixed_height = 0;
        int fixed_size_count = 0;

        utility::algorithm::tuples_zip(
            [&](auto& child_renderer, auto& child_view)
            {
                BoxConstraints constraints {
                    .min_size = glm::vec2 { 0 },
                    .max_size = glm::vec2 { rect_size.x, available_height },
                };

                const SizeVariant child_size =
                    child_renderer.layout(constraints, child_view);

                if (child_size.is_fixed())
                {
                    const auto fixed_size =
                        std::get<SizeVariant::Fixed>(child_size.variant);
                    fixed_height += fixed_size.size.y;
                    fixed_size_count++;

                    available_height -= fixed_size.size.y;
                }
            },
            children,
            view.children
        );

        const auto filled_size_count = sizeof...(TChildView) - fixed_size_count;

        const double filled_space = (rect_size.y - fixed_height);
        const double height_per_filled = filled_space / filled_size_count;
        available_height = rect.size().y;

        double y_offset = 0;

        utility::algorithm::tuples_zip(
            [&](auto& child_renderer, auto& child_view)
            {
                BoxConstraints constraints {
                    .min_size = glm::vec2 { 0 },
                    .max_size = glm::vec2 { rect_size.x, available_height }
                };
                const SizeVariant child_size =
                    child_renderer.layout(constraints, child_view);

                auto top_left = glm::vec2 { rect.left(), rect.top() - y_offset };

                if (child_size.is_filled())
                {
                    auto draw_rect = Rect::from_top_left(
                        top_left, glm::vec2 { rect_size.x, height_per_filled }
                    );
                    child_renderer.render(state, render_manager, draw_rect, child_view);

                    y_offset += height_per_filled;
                }
                else if (child_size.is_fixed())
                {
                    auto fixed_size = std::get<SizeVariant::Fixed>(child_size.variant);
                    auto draw_rect = Rect::from_top_left(top_left, fixed_size.size);
                    child_renderer.render(state, render_manager, draw_rect, child_view);

                    y_offset += draw_rect.size().y;
                }
            },
            children,
            view.children
        );
    }
};
}