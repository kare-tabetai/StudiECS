#pragma once
#include "Type.h"
#include "IdGenerator.h"
#include <boost/hana.hpp>

namespace Util {

template<CdOrEntityConcept... T>
Archetype TypeListToArchetype(const hana_tuple<T...>& type_list)
{
    Archetype m_archetype;
    boost::hana::for_each(type_list, [&m_archetype](auto t) {
        using T = typename decltype(t)::type;
        m_archetype.push_back(TypeIDGenerator<T>::id());
    });

    return m_archetype;
}

/// \brief start <= address && address < end‚È‚çtrue
bool IsInRange(const void* address, void* start, void* end) {
    return start <= address && address < end;
}

}