#pragma once
#include "Type.h"
#include "CdIdGenerator.h"
#include <boost/hana.hpp>

namespace Util {

template<typename... T>
Archetype TypeListToArchetype(const boost::hana::tuple<T...>& type_list)
{
    Archetype m_archetype;
    boost::hana::for_each(type_list, [&m_archetype](auto t) {
        using T = typename decltype(t)::type;
        m_archetype.push_back(CdIdGenerator<T>::m_id());
    });

    return m_archetype;
}

}