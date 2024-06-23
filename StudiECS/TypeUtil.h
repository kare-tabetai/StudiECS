#pragma once
#include "TypeIDGenerator.h"
#include <boost/hana.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>

namespace TypeUtil {

template<bool b>
using BoolToType = std::integral_constant<bool, b>;

template<class... Args>
constexpr auto MakeTuple()
{
    return boost::hana::tuple_t<Args...>;
}

constexpr auto AddTypes(auto lhs, auto rhs)
{
    return boost::hana::concat(lhs, rhs);
}

constexpr auto RemoveTypes(auto input_type_list, auto remove_type_list)
{
    return boost::hana::remove_if(input_type_list, [remove_type_list](auto input_type) {
        return boost::hana::contains(remove_type_list, input_type);
    });
}

constexpr bool HasAnyTypes(auto input_type_list, auto search_type_list)
{
    return boost::hana::any_of(search_type_list, [input_type_list](auto t) {
        return boost::hana::contains(input_type_list, t);
    });
}

constexpr bool HasAllTypes(auto input_type_list, auto search_type_list)
{
    return boost::hana::all_of(search_type_list, [input_type_list](auto t) {
        return boost::hana::contains(input_type_list, t);
    });
}

constexpr auto SortTypeList(auto input_type_list)
{
    auto id_sort_func = [](auto a, auto b) constexpr {
        using TypeA = decltype(a);
        using TypeB = decltype(b);
        constexpr uint32 a_val = TypeIDGenerator<TypeA>::id();
        constexpr uint32 b_val = TypeIDGenerator<TypeB>::id();
        constexpr auto a_size = boost::hana::size_c<a_val>;
        constexpr auto b_size = boost::hana::size_c<b_val>;
        return a_size < b_size;
    };
    return boost::hana::sort(input_type_list, id_sort_func);
}

constexpr bool TypeToBool(auto b)
{
    return decltype(b)::value;
}

constexpr bool IsCantCDType(auto type)
{
    return boost::hana::traits::is_void(type)
        || boost::hana::traits::is_pointer(type)
        || boost::hana::traits::is_reference(type);
}

constexpr auto SanitizeTuple(auto test_types)
{
    return boost::hana::remove_if(test_types, [](auto t) {
        return BoolToType<IsCantCDType(t)>();
    });
}

constexpr auto ToPointerTuple(auto test_types)
{
    return boost::hana::transform(test_types, [](auto t) {
        return boost::hana::traits::add_pointer(t);
    });
}

}