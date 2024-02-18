#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace division_engine::utility::algorithm
{
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

template<class TCallback, class... TArgs>
void tuple_foreach(TCallback& callback, const std::tuple<TArgs...>& tuple);

namespace
{
    template<std::size_t Index, class TCallback, class... TArgs>
    struct __Foreach__
    {
        static void tuple_foreach(TCallback& callback, const std::tuple<TArgs...>& tuple)
        {
            const std::size_t idx = sizeof...(TArgs) - Index;
            callback.operator()<idx>(std::get<idx>(tuple));
            __Foreach__<Index - 1, TCallback, TArgs...>::tuple_foreach(callback, tuple);

        }
    };

    template<class TCallback, class... TArgs>
    struct __Foreach__<0, TCallback, TArgs...>
    {
        static void tuple_foreach(TCallback&, const std::tuple<TArgs...>&) {}
    };


}

template<class TCallback, class... TArgs>
void tuple_foreach(TCallback& callback, const std::tuple<TArgs...>& tuple)
{
    __Foreach__<sizeof...(TArgs), TCallback, TArgs...>::tuple_foreach(callback, tuple);
}
}