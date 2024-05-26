#pragma once

#include "division_engine/canvas/box_constraints.hpp"
#include "division_engine/canvas/rect.hpp"
#include "division_engine/canvas/render_manager.hpp"
#include "division_engine/canvas/size.hpp"
#include "division_engine/utility/algorithm.hpp"
#include "division_engine/utility/meta.hpp"

#include <algorithm>
#include <concepts>
#include <tuple>
#include <type_traits>

namespace division_engine::canvas::view_tree
{
template<typename T, typename = void>
constexpr bool has_renderer = false;

template<typename T>
constexpr bool has_renderer<T, std::void_t<typename T::Renderer>> = true;

template<typename V>
concept View =
    has_renderer<V> &&
    requires(
        V v,
        typename V::Renderer r,
        const BoxConstraints& box_constraints,
        State& state,
        RenderManager& render_manager,
        Rect& rect
    ) {
        typename V::Renderer;
        std::constructible_from<typename V::Renderer, State&, RenderManager&, const V&>;
        {
            r.layout(box_constraints, v)
        } -> std::same_as<Size>;
        {
            r.render(state, render_manager, rect, v)
        };
    };

template<typename V>
concept SingleChildView =
    View<V> && std::copy_constructible<V> &&
    requires(V v) {
        {
            std::remove_reference_t<decltype(V::child)>(v.child)
        } -> View;
    };

template<template<typename> typename V, typename... Children>
struct is_multi_child_view
{
    static const bool value = (... && View<Children>);
};

template<typename... Args>
constexpr bool all_are_views()
{
    return (... && View<Args>);
}

template<typename... V>
constexpr inline bool assert_all_are_views()
{
    return (
        ... &&
        []()
        {
            static_assert(View<V>);
            return View<V>;
        }()
    );
}

template<typename>
struct is_view_tuple : std::false_type
{
};

template<typename... Args>
struct is_view_tuple<std::tuple<Args...>>
  : std::integral_constant<bool, all_are_views<Args...>()>
{
};

template<typename V>
concept MultiChildView = View<V> && is_view_tuple<decltype(V::children)>();
}