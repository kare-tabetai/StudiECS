#pragma once
#include "Type.h"
#include "IdGenerator.h"
#include "Concept.h"
#include "Util.h"
#include "TypeUtil.h"
#include "Entity.h"
#include <boost/hana.hpp>

namespace Util {

constexpr bool IsCantCDType(auto type)
{
    return boost::hana::traits::is_void(type)
        || boost::hana::traits::is_pointer(type)
        || boost::hana::traits::is_reference(type)
        || boost::hana::type_c<Entity> == type;
}

template<class... T>
constexpr auto PushFrontEntity(hana_tuple<T...> input_type_list)
{
    constexpr auto entity_type_list = boost::hana::tuple_t<Entity>;
    return TypeUtil::AddTypes(entity_type_list, input_type_list);
}

template<class... T>
constexpr auto RemoveCantCDType(hana_tuple<T...> input_type_list)
{
    return boost::hana::remove_if(input_type_list, [](auto t) {
        return TypeUtil::BoolToType<IsCantCDType(t)>();
    });
}

template<class... T>
constexpr auto SanitizeTypeList(hana_tuple<T...> input_type_list)
{
    return PushFrontEntity(TypeUtil::SortTypeList(RemoveCantCDType(TypeUtil::Unique(input_type_list))));
}

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