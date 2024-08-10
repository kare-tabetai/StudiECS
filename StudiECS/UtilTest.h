#pragma once
#include "Util.h"
#include "TupleUtil.h"

void UtilTest()
{
    std::cout << "UtilTest\n";

    constexpr auto test_types = boost::hana::tuple_t<int, double,  int, char>;
    Archetype m_archetype = Util::TypeListToArchetype(test_types);

    auto lhs_tuple = std::make_tuple(1, 5.5, 32);
    auto rhs_tuple = std::make_tuple(3, 4.5, 32);

    auto add_lambda = [](auto& lhs, auto& rhs) {
        lhs += rhs;
    };
    TupleUtil::ApplyTuples(add_lambda, lhs_tuple, rhs_tuple);
}