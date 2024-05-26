
#include <tuple>
#include <type_traits>

namespace division_engine::utility::meta
{
template<typename>
struct is_tuple : std::false_type
{
};
template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type
{
};

template<typename T>
concept tuple_like = is_tuple<T>();
}