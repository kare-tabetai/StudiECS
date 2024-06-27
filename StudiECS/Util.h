#pragma once
#include "Type.h"
#include "IdGenerator.h"
#include <boost/hana.hpp>

namespace Util {

template<typename... T>
Archetype TypeListToArchetype(const hana_tuple<T...>& type_list)
{
    Archetype m_archetype;
    boost::hana::for_each(type_list, [&m_archetype](auto t) {
        using T = typename decltype(t)::type;
        m_archetype.push_back(CdIdGenerator<T>::id());
    });

    return m_archetype;
}

}