#pragma once
#include "Type.h"
#include "Concept.h"
#include "TypeUtil.h"

template<CdConcept... CD>
using EntityCdPtrTuple = std::tuple<Entity*,CD*...>;

namespace Util {

template<class... T>
constexpr auto PushFrontEntity(hana_tuple<T...> input_type_list)
{
    constexpr auto entity_type_list = boost::hana::tuple_t<Entity>;
    return TypeUtil::AddTypes(entity_type_list, input_type_list);
}


template<class... T>
constexpr auto SanitizeTypeList(hana_tuple<T...> input_type_list)
{
    return PushFrontEntity(TypeUtil::SortTypeList(TypeUtil::Unique(input_type_list)));
}

template<CdOrEntityConcept... T>
Archetype TypeListToArchetype(const hana_tuple<T...>& type_list)
{
    Archetype archetype;
    boost::hana::for_each(type_list, [&archetype](auto t) {
        using T = typename decltype(t)::type;
        archetype.push_back(TypeIDGenerator<T>::id());
    });

    return archetype;
}

/// \brief start <= address && address < end‚È‚çtrue
bool IsInRange(const void* address, void* start, void* end) {
    return start <= address && address < end;
}

}