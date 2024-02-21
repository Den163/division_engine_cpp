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
void tuple_foreach(const TCallback& callback, std::tuple<TArgs...>& tuple)
{
    std::apply(
        [&](TArgs&... el) 
        { 
            ([&]() { callback(el); }(), ...); 
        }, 
        tuple
    );
}
}