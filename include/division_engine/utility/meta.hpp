namespace division_engine::utility::meta
{
template<bool value, typename TrueType, typename FalseType>
struct type_selector
{
};

template<typename TrueType, typename FalseType>
struct type_selector<true, TrueType, FalseType>
{
    using type = TrueType;
};

template<typename TrueType, typename FalseType>
struct type_selector<false, TrueType, FalseType>
{
    using type = FalseType;
};


template<bool value, typename TrueType, typename FalseType>
using type_selector_t = typename type_selector<value, TrueType, FalseType>::type;
}