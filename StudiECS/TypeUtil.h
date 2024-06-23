#pragma once
#include "TypeIDGenerator.h"
#include <boost/hana.hpp>

namespace TypeUtil {

template<bool b>
using BoolToType = std::integral_constant<bool, b>;

template<class... Args>
constexpr auto MakeTypeList()
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

template<class... T>
constexpr auto SortTypeList(boost::hana::tuple<T...> input_type_list)
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

template<class... T>
constexpr auto Unique(boost::hana::tuple<T...> input_type_list)
{
    //MEMO:hana::unique‚Ísort‚µ‚Ä‚©‚ç‚Å‚Í‚È‚¢‚Æ‚¤‚Ü‚­“®‚©‚È‚¢‚ç‚µ‚¢
    return boost::hana::unique(SortTypeList(input_type_list));
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

template<class... T>
constexpr auto RemoveCantCDType(boost::hana::tuple<T...> input_type_list)
{
    return boost::hana::remove_if(input_type_list, [](auto t) {
        return BoolToType<IsCantCDType(t)>();
    });
}

template<class... T>
constexpr auto SanitizeTypeList(boost::hana::tuple<T...> input_type_list)
{
    return SortTypeList(RemoveCantCDType(Unique(input_type_list)));
}

template<class... T>
constexpr auto ToPointerTypeList(boost::hana::tuple<T...> input_type_list)
{
    return boost::hana::transform(input_type_list, [](auto t) {
        return boost::hana::traits::add_pointer(t);
    });
}

constexpr uint32 GetAlignedTypeSize(uint32 type_size, uint32 align_size)
{
    return (type_size + align_size - 1) / align_size * align_size;
}

constexpr uint32 GetAlignedIndexPtr(uint32 type_size, uint32 align_size)
{
    return (type_size + align_size - 1) / align_size * align_size;
}

}