#pragma once
#include "Util.h"

void UtilTest()
{
    auto test_types = boost::hana::tuple_t<int, double, void, int, char*, char, void, int&>;

    Util::typeListToArchetypeImpl(test_types);
}