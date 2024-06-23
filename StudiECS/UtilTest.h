#pragma once
#include "Util.h"

void UtilTest()
{
    std::cout << "UtilTest\n";

    auto test_types = boost::hana::tuple_t<int, double, void, int, char*, char, void, int&>;
    Archetype archetype = Util::TypeListToArchetype(test_types);

}