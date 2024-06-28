#pragma once
#include <utility>
#include <type_traits>

namespace EnumUtil {

template<typename T>
concept IsEnum = std::is_enum_v<T>;

template<IsEnum Enum>
constexpr std::underlying_type_t<Enum> ToBase(Enum flag)
{
    return static_cast<std::underlying_type_t<Enum>>(flag);
}

/// \note TODO: 論理演算の戻り値をenumラッパークラスにして、
/// キャスト演算子でboolにもenumにも変換できるようにするとToBoolが不要にできるかも
template<IsEnum Enum>
constexpr bool ToBool(Enum flag)
{
    return ToBase(flag) != 0;
}

template<IsEnum Enum>
constexpr Enum operator|(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) | ToBase(rhs));
}

template<IsEnum Enum>
constexpr Enum& operator|=(Enum& lhs, Enum rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

template<IsEnum Enum>
constexpr Enum operator&(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) & ToBase(rhs));
}

template<IsEnum Enum>
constexpr Enum& operator&=(Enum& lhs, Enum rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

template<IsEnum Enum>
constexpr Enum operator^(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) ^ ToBase(rhs));
}

template<IsEnum Enum>
constexpr Enum& operator^=(Enum& lhs, Enum rhs)
{
    lhs = lhs ^ rhs;
    return lhs;
}

template<IsEnum Enum>
constexpr Enum operator~(Enum flag)
{
    return static_cast<Enum>(~ToBase(flag));
}

template<IsEnum Enum>
constexpr bool HasAnyFlags(Enum flag, Enum check_flag)
{
    return (flag & check_flag) != 0;
}

template<IsEnum Enum>
constexpr bool HasAllFlags(Enum flag, Enum check_flag)
{
    return (flag & check_flag) == check_flag;
}

}

using namespace EnumUtil;
