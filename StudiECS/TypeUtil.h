#pragma once
#include "TypeIDGenerator.h"
#include <boost/hana.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>

namespace TypeUtil {

template <bool b>
using BoolToType = std::integral_constant<bool, b>;

template<class... Args>
constexpr auto MakeTuple() {
    return boost::hana::tuple_t<Args...>;
}

constexpr auto SortTypeList(auto in_type_list)
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
    return boost::hana::sort(in_type_list, id_sort_func);
}

constexpr bool TypeToBool(auto b) {
    return decltype(b)::value;
}

constexpr bool IsCantCDType(auto type)
{
    return boost::hana::traits::is_void(type)
        || boost::hana::traits::is_pointer(type)
        || boost::hana::traits::is_reference(type);
}

constexpr auto SanitizeTuple(auto types)
{
    return boost::hana::remove_if(types, [](auto t) {
        return BoolToType<IsCantCDType(t)>();
    });
}

constexpr auto ToPointerTuple(auto types)
{
    return boost::hana::transform(types, [](auto t) {
        return boost::hana::traits::add_pointer(t);
    });
}

}