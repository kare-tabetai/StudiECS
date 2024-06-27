#pragma once
#include "ArchetypeInfo.h"
#include "Entity.h"
#include "Type.h"
#include "TypeUtil.h"
#include "SparseSet.h"
#include <memory>
#include <unordered_map>
#include "EntityData.h"

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
    // \brief CD情報の実体 [CdNumber]
    SparseSet<OwnerPtr<TypeInfo>> m_cd_infos;

    // \brief Archetype情報の実体 [ArchetypeNumber]
    SparseSet<OwnerPtr<ArchetypeInfo>> m_archetype_infos; 

    // \brief Entityから持っているCDを探す用の参照
    std::unordered_map<Entity, EntityData> m_entity_datas;

    // \brief CD->所属Archetype検索用の参照
    using ArchetypeMap = std::unordered_map<ArchetypeNumber, RefPtr<ArchetypeInfo>>;
    std::vector<ArchetypeMap> m_component_index;//m_component_index[cd_number]

    template<class CD>
    RefPtr<TypeInfo> getOrRegisterTypeInfo()
    {
        CdNumber cd_number = CdIdGenerator<CD>::number();
        if (m_cd_infos.Has(cd_number)) {
            return m_cd_infos[cd_number];
        } else {
            auto&& type_info_ptr = std::make_shared<TypeInfo>(TypeInfo::Make<CD>());
            RefPtr<TypeInfo> ret_ptr = type_info_ptr;
            m_cd_infos[cd_number] = std::move(type_info_ptr);
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
    ArchetypeInfo& registerArchetypeInfo(ArchetypeNumber archetype_number, const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        Archetype arche_type = Util::TypeListToArchetype(sanitized_type_list);
        auto&& info = std::make_shared<ArchetypeInfo>(archetype_number, arche_type, types_ref);
        m_archetype_infos[archetype_number] = std::move(info);
        return *m_archetype_infos[archetype_number];
    }

    template<class... T>
    ArchetypeInfo& getOrRegisterArchetypeInfo(const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        assert(TypeUtil::IsFrontType(boost::hana::type_c<Entity>, sanitized_type_list));

        ArchetypeNumber archetype_number = ArchetypeIDGenerator<decltype(sanitized_type_list)>::number();
        if (m_archetype_infos.Has(archetype_number)) {
            return *m_archetype_infos[archetype_number];
        } else {
            return registerArchetypeInfo(archetype_number, sanitized_type_list, types_ref);
        }
    }

};