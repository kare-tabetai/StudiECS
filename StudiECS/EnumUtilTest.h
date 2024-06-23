#pragma once
#include "EnumUtil.h"
#include "Type.h"

enum class TestEnum {
    None = 0,
    Hoge = 1 << 0,
    Huga = 1 << 1,
    HogeHoge = 1 << 2,
};

enum class UnderlyingTestEnum : uint64 {
    Hoge = 0,
    Huga = 1 << 0,
    HogeHoge = 1 << 1,
};

void EnumUtilTest()
{
    constexpr TestEnum or_test = TestEnum::Hoge | TestEnum::HogeHoge;
    constexpr TestEnum and_test = or_test & TestEnum::Huga;
    constexpr TestEnum xor_test = or_test ^ (TestEnum::Huga | TestEnum::HogeHoge);
    constexpr bool true_enum = EnumUtil::ToBool(or_test);
    constexpr bool false_enum = EnumUtil::ToBool(and_test);
}