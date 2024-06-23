#pragma once
#include "Type.h"
#include "CdIdGenerator.h"
#include <boost/hana.hpp>

namespace Util {

template<typename... T>
Archetype TypeListToArchetype(const boost::hana::tuple<T...>& type_list)
{
    Archetype archetype;
    boost::hana::for_each(type_list, [&archetype](auto t) {
        using T = typename decltype(t)::type;
        archetype.push_back(CdIdGenerator<T>::id());
    });

    return archetype;
}

}