#pragma once
#include "ArchetypeInfo.h"
#include "Entity.h"
#include "Type.h"
#include "TypeUtil.h"
#include <memory>
#include <unordered_map>

/// \brief 一番rootになるクラスここから全部操作する
/// \note 型IDをhashではなくて単純な加算値にしたほうがstd::vectorにできて早そうな気がする
class World {
public:
    template<class... Args>
    Entity CreateEntity()
    {
        constexpr auto type_list = TypeUtil::MakeTypeList<Args...>();
        constexpr auto sanitized = TypeUtil::SanitizeTypeList(type_list);
        TypeInfoRefContainer type_info_refs = registerTypeInfos(sanitized);
        auto& arche_info = getOrRegisterArchetypeInfo(sanitized, type_info_refs);
        

        return Entity::Invalid();
    }

private:
    TypeInfoContainer m_type_infos;

    std::vector<OwnerPtr<ArchetypeInfo>> m_archetype_infos;//m_archetype_infos[archetype_number]
    std::unordered_map<Entity, RefPtr<ArchetypeInfo>> m_entity_to_archetype;

    using ArchetypeMap = std::unordered_map<ArchetypeNumber, RefPtr<ArchetypeInfo>>;
    std::vector<ArchetypeMap> m_component_index;//m_component_index[cd_number]

    template<class CD>
    RefPtr<TypeInfo> getOrRegisterTypeInfo()
    {
        CdNumber cd_number = CdIdGenerator<CD>::number();
        if (cd_number < m_type_infos.size()) {
            return m_type_infos[cd_number];
        } else {
            assert(m_type_infos.size() == cd_number);
            auto&& type_info_ptr = std::make_shared<TypeInfo>(TypeInfo::Make<CD>());
            RefPtr<TypeInfo> ret_ptr = type_info_ptr;
            m_type_infos.push_back(std::move(type_info_ptr));
            return ret_ptr;
        }
    }
    
    template<class... T>
    TypeInfoRefContainer registerTypeInfos(const hana_tuple<T...>& sanitized_type_list)
    {
        TypeInfoRefContainer refs;
        boost::hana::for_each(sanitized_type_list, [this, &refs](auto t) {
            using T = typename decltype(t)::type;
            auto weak_tpr = getOrRegisterTypeInfo<T>();
            refs.push_back(weak_tpr);
        });
        return refs;
    }
    
    template<class... T>
    ArchetypeInfo& registerArchetypeInfo(ArcheTypeID archetype_number, const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        Archetype arche_type = Util::TypeListToArchetype(sanitized_type_list);
        auto&& info = std::make_shared<ArchetypeInfo>(archetype_number, arche_type, types_ref);
        m_archetype_infos.push_back(std::move(info));
        return *m_archetype_infos.back();
    }

    template<class... T>
    ArchetypeInfo& getOrRegisterArchetypeInfo(const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        ArchetypeNumber archetype_number = ArchetypeIDGenerator<decltype(sanitized_type_list)>::number();
        if (archetype_number < m_archetype_infos.size()) {
            return *m_archetype_infos[archetype_number];
        } else {
            assert(m_archetype_infos.size() == archetype_number);
            return registerArchetypeInfo(archetype_number, sanitized_type_list, types_ref);
        }
    }

};