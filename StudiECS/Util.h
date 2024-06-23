#pragma once
#include "Type.h"
#include "CdIdGenerator.h"
#include <boost/hana.hpp>

namespace Util {

Archetype TypeListToArchetype(const auto& type_list)
{
    Archetype archetype;
    //typeListToArchetypeImpl(type_list);
    return archetype;
}

template<typename... T>
void typeListToArchetypeImpl(const boost::hana::tuple<T...>& type_list)
{
    // Œ^‚ð•\Ž¦‚·‚é—á
    std::initializer_list<int> swallow = {
        (
            void(
                std::cout << CdIdGenerator<typename T::type>::id() << "\n"),
            0)...
    };
}

}