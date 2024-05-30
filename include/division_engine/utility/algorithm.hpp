#pragma once

#include <__tuple/tuple_element.h>
#include <__utility/integer_sequence.h>
#include <algorithm>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace division_engine::utility::algorithm
{
namespace
{
template<size_t n_element, class TCallback, class... TTuple>
void __tuples_zip_impl(const TCallback& callback, TTuple&... tuple)
{
    callback(std::get<n_element>(tuple)...);
}

template<size_t... n_element, class TCallback, class... TTuple>
void __tuples_zip_impl(
    std::index_sequence<n_element...>,
    const TCallback& callback,
    TTuple&... tuple
)
{
    (__tuples_zip_impl<n_element>(callback, tuple...), ...);
}

template<size_t... n_element, class TCallback, class TTuple>
auto __tuple_transform_impl(
    std::index_sequence<n_element...>,
    const TCallback& callback,
    const TTuple& tuple
)
{
    return std::make_tuple(callback(
        (typename std::tuple_element<n_element, TTuple>::type &)
        (std::get<n_element>(tuple))
    )...);
}
}

template<class TContainer, typename TElement, typename TComparer>
auto sorted_insert(
    TContainer& insert_container,
    const TElement& element,
    const TComparer& comparer = [](const auto& x, const auto& y) { return x < y; }
)
{
    auto lower_bound = std::lower_bound(
        insert_container.begin(), insert_container.end(), element, comparer
    );

    if (lower_bound == insert_container.end() || comparer(*lower_bound, element))
    {
        lower_bound = insert_container.insert(lower_bound, element);
    }

    return lower_bound;
}

template<typename TCallback, typename... TArgs>
constexpr bool is_foreach_callback =
    (std::is_convertible<TCallback, std::function<void(TArgs&)>>() || ...);

template<typename TCallback, typename... TArgs>
constexpr bool is_foreach_with_indexer_callback =
    (std::is_convertible<TCallback, std::function<void(TArgs&, size_t)>>() || ...);

template<class TCallback, class... TArgs>
    requires(is_foreach_callback<TCallback, TArgs...>)
void tuple_foreach(const TCallback& callback, std::tuple<TArgs...>& tuple)
{
    std::apply([&](TArgs&... el) { ([&]() { callback(el); }(), ...); }, tuple);
}

template<class TCallback, class... TArgs>
    requires(is_foreach_with_indexer_callback<TCallback, TArgs...>)
void tuple_foreach(const TCallback& callback, std::tuple<TArgs...>& tuple)
{
    size_t i = 0;
    std::apply([&](TArgs&... el) { ([&]() { callback(el, i++); }(), ...); }, tuple);
}

template<class TCallback, class... TTuple>
void tuples_zip_foreach(const TCallback& callback, TTuple&... tuple)
{
    constexpr size_t min_tuple_size =
        std::min(std::initializer_list<size_t> { std::tuple_size<TTuple>()... });

    __tuples_zip_impl(std::make_index_sequence<min_tuple_size>(), callback, tuple...);
}

template<class TMapCallback, class... TArgs>
decltype(auto)
tuple_transform(const TMapCallback& callback, const std::tuple<TArgs...>& tuple)
{
    return __tuple_transform_impl(
        std::make_index_sequence<sizeof...(TArgs)>(), callback, tuple
    );
}
}