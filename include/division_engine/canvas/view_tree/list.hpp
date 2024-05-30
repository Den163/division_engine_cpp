#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/canvas/state.hpp"
#include "division_engine/canvas/view_tree/view.hpp"
#include "division_engine/utility/algorithm.hpp"

#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>

namespace division_engine::canvas::view_tree
{
enum class Direction
{
    Horinzontal,
    Vertical
};

namespace
{
template<Direction elements_direction, View... TChildView>
struct __BaseListView
{
    struct Renderer;
    using base_type = __BaseListView<elements_direction, TChildView...>;

    std::tuple<TChildView...> children;

    __BaseListView(TChildView... children)
      : children(std::make_tuple(children...))
    {
    }
};
}

template<typename... T>
struct HorizontalList : __BaseListView<Direction::Horinzontal, T...>
{
    HorizontalList(T... children)
      : __BaseListView<Direction::Horinzontal, T...>(children...)
    {
    }
};

template<typename... T>
struct VerticalList : __BaseListView<Direction::Vertical, T...>
{
    VerticalList(T... children)
      : __BaseListView<Direction::Vertical, T...>(children...)
    {
    }
};

template<Direction elements_direction, View... TChildView>
struct __BaseListView<elements_direction, TChildView...>::Renderer
{
    using view_type = std::conditional_t<
        elements_direction == Direction::Horinzontal,
        HorizontalList<TChildView...>,
        VerticalList<TChildView...>>;

    std::tuple<typename TChildView::Renderer...> children;

    Renderer(State& state, RenderManager& render_manager, const view_type& view)
      : children(utility::algorithm::tuple_transform(
            [&state, &render_manager](auto& v)
            {
                using child_view_t = std::remove_reference_t<decltype(v)>;
                using child_renderer_t = typename child_view_t::Renderer;
                return child_renderer_t { state, render_manager, v };
            },
            view.children
        ))
    {
    }

    Size layout(const BoxConstraints& constraints, const view_type& view)
    {
        return Size::unconstrainted();
    }

    void
    render(State& state, RenderManager& render_manager, Rect& rect, const view_type& view)
    {
        const auto rect_size = rect.size();

        glm::vec2 available_size = rect_size;
        glm::vec2 all_fixed_size = glm::vec2 { 0 };
        int fixed_size_count = 0;

        utility::algorithm::tuples_zip_foreach(
            [&](auto& child_renderer, auto& child_view)
            {
                BoxConstraints constraints {
                    .min_size = glm::vec2 { 0 },
                    .max_size = glm::vec2 { available_size.x, available_size.y },
                };

                const Size child_size = child_renderer.layout(constraints, child_view);

                if (!is_unconstrainted<main_direction()>(child_size))
                {
                    fixed_size_count++;

                    auto& available_size_comp =
                        get_vec_component_by_direction<main_direction()>(available_size);
                    auto& child_size_comp =
                        get_vec_component_by_direction<main_direction()>(child_size);
                    auto& all_fixed_size_comp =
                        get_vec_component_by_direction<main_direction()>(all_fixed_size);

                    available_size_comp -= child_size_comp;
                    all_fixed_size_comp += child_size_comp;
                }
            },
            children,
            view.children
        );

        constexpr size_t child_count = std::tuple_size_v<decltype(children)>;
        const auto filled_size_count = child_count - fixed_size_count;

        const glm::vec2 filled_space = (rect_size - all_fixed_size);
        glm::vec2 size_per_filled = rect_size;

        auto& filled_space_comp =
            get_vec_component_by_direction<main_direction()>(filled_space);
        auto& size_per_filled_comp =
            get_vec_component_by_direction<main_direction()>(size_per_filled);
        size_per_filled_comp = filled_space_comp / filled_size_count;

        available_size = rect_size;

        glm::vec2 offset { 0 };

        utility::algorithm::tuples_zip_foreach(
            [&](View auto& child_view, auto& child_renderer)
            {
                BoxConstraints constraints {
                    .min_size = glm::vec2 { 0 },
                    .max_size = available_size,
                };
                const Size child_size = child_renderer.layout(constraints, child_view);

                auto top_left =
                    glm::vec2 { rect.left() + offset.x, rect.top() - offset.y };

                float& offset_comp =
                    get_vec_component_by_direction<main_direction()>(offset);
                if (is_unconstrainted<elements_direction>(child_size))
                {
                    auto draw_rect = Rect::from_top_left(top_left, size_per_filled);
                    child_renderer.render(state, render_manager, draw_rect, child_view);

                    float size_per_filled_comp =
                        get_vec_component_by_direction<main_direction()>(size_per_filled);

                    offset_comp += size_per_filled_comp;
                }
                else
                {
                    auto draw_rect = Rect::from_top_left(top_left, child_size);
                    auto draw_rect_size = draw_rect.size();
                    const float draw_rect_size_comp =
                        get_vec_component_by_direction<main_direction()>(draw_rect_size);

                    child_renderer.render(state, render_manager, draw_rect, child_view);
                    offset_comp += draw_rect_size_comp;
                }
            },
            view.children,
            children
        );
    }

private:
    template<Direction direction>
    constexpr static bool is_unconstrainted(const Size& size)
    {
        return direction == Direction::Horinzontal ? size.width_unconstrainted()
                                                   : size.height_unconstrainted();
    }

    constexpr static Direction main_direction() { return elements_direction; }

    constexpr static Direction cross_direction()
    {
        return elements_direction == Direction::Horinzontal
                   ? Direction::Vertical
                   : Direction::Horinzontal;
    }

    template<Direction direction>
    constexpr static float& get_vec_component_by_direction(glm::vec2& v)
    {
        return direction == Direction::Horinzontal ? v.x : v.y;
    }

    template<Direction direction>
    constexpr static const float& get_vec_component_by_direction(const glm::vec2& v)
    {
        return direction == Direction::Horinzontal ? v.x : v.y;
    }
};
}