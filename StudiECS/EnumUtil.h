#pragma once
#include <utility>
#include <type_traits>

namespace EnumUtil {

template<typename T>
concept EnumConcept = std::is_enum_v<T>;

template<EnumConcept Enum>
constexpr std::underlying_type_t<Enum> ToBase(Enum flag)
{
    return static_cast<std::underlying_type_t<Enum>>(flag);
}

template<EnumConcept Enum>
class EnumWrapper {
    Enum value;

public:
    // コンストラクタ
    EnumWrapper(Enum val)
        : value(val)
    {
    }

    // Enum型への自動キャスト
    operator Enum() const
    {
        return value;
    }

    // bool型への自動キャスト
    explicit operator bool() const
    {
        return static_cast<std::underlying_type_t<Enum>>(value) != 0;
    }
};

/// \note TODO: 論理演算の戻り値をenumラッパークラスにして、
/// キャスト演算子でboolにもenumにも変換できるようにするとToBoolが不要にできるかも
template<EnumConcept Enum>
constexpr bool ToBool(Enum flag)
{
    return ToBase(flag) != 0;
}

template<EnumConcept Enum>
constexpr Enum operator|(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) | ToBase(rhs));
}

template<EnumConcept Enum>
constexpr Enum& operator|=(Enum& lhs, Enum rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

template<EnumConcept Enum>
constexpr Enum operator&(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) & ToBase(rhs));
}

template<EnumConcept Enum>
constexpr Enum& operator&=(Enum& lhs, Enum rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

template<EnumConcept Enum>
constexpr Enum operator^(Enum lhs, Enum rhs)
{
    return static_cast<Enum>(ToBase(lhs) ^ ToBase(rhs));
}

template<EnumConcept Enum>
constexpr Enum& operator^=(Enum& lhs, Enum rhs)
{
    lhs = lhs ^ rhs;
    return lhs;
}

template<EnumConcept Enum>
constexpr Enum operator~(Enum flag)
{
    return static_cast<Enum>(~ToBase(flag));
}

template<EnumConcept Enum>
constexpr bool HasAnyFlags(Enum flag, Enum check_flag)
{
    return (flag & check_flag) != 0;
}

template<EnumConcept Enum>
constexpr bool HasAllFlags(Enum flag, Enum check_flag)
{
    return (flag & check_flag) == check_flag;
}

}

using namespace EnumUtil;
