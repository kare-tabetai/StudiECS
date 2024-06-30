#pragma once
#include "ArchetypeInfo.h"
#include "Entity.h"
#include "Type.h"
#include "TypeUtil.h"
#include "SparseSet.h"
#include "Constant.h"
#include "Concept.h"
#include <memory>
#include <unordered_map>
#include <vector>

/// \brief 一番rootになるクラスここから全部操作する
class World {
public:
    World() {
        m_number = s_world_number_counter;
        s_world_number_counter++;
        assert(s_world_number_counter != kUint8Max);
    }

    template<CdConcept... Args>
    Entity CreateEntity()
    {
        constexpr auto type_list = TypeUtil::MakeTypeList<Args...>();
        constexpr auto sanitized = Util::SanitizeTypeList(type_list);

        //m_cd_infosにCDを設定
        TypeInfoRefContainer type_info_refs = registerTypeInfos(sanitized);

        //m_archetype_infosにアーキタイプを設定
        RefPtr<ArchetypeInfo> archetype_ref = getOrRegisterArchetypeInfo(sanitized, type_info_refs);
        Entity entity = archetype_ref->CreateEntity();

        //m_entity_datasにアーキタイプを設定
        assert(m_entity_datas.count(entity) == 0);
        m_entity_datas.emplace(entity, archetype_ref);

        // m_component_indexにアーキタイプを設定
        for (const auto& type_info : type_info_refs) {
            if (m_component_index.count(type_info->GetID())== 0) {
                m_component_index.emplace(type_info->GetID() ,ArchetypeMap());
            }
            m_component_index[type_info->GetID()][archetype_ref->GetNumber()] = archetype_ref;

        }
        return entity;
    }

    template<CdConcept CD>
    CD* Get(Entity entity) {
        PtrTuple<CD> cd_ptr_tuple = GetTypes<CD>(entity);
        return std::get<0>(cd_ptr_tuple);
    }

    template<CdConcept... CD>
    PtrTuple<CD...> GetTypes(Entity entity)
    {
        return m_archetype_infos[entity.GetIndex().archetype_number]->GetTypes<CD...>(entity.GetIndex());
    }

    template<CdOrEntityConcept CdOrEntity>
    std::vector<ArrayView<CdOrEntity>> GetCdArray() {
        m_component_index.find()
            //TODO:
    }

    void Shrink() {
        //未使用になっているchunkなどを削除する
        // denseを指していないsparse setのsparse配列をdenseを指しているところまで縮める
    }

private:

    template<CdOrEntityConcept CD>
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

    template<CdOrEntityConcept... T>
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

    template<CdOrEntityConcept... T>
    RefPtr<ArchetypeInfo> registerArchetypeInfo(ArchetypeNumber archetype_number, const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        Archetype arche_type = Util::TypeListToArchetype(sanitized_type_list);
        auto&& info = std::make_shared<ArchetypeInfo>(archetype_number, arche_type, types_ref, m_number);
        m_archetype_infos[archetype_number] = std::move(info);
        return m_archetype_infos[archetype_number];
    }

    template<CdOrEntityConcept... T>
    RefPtr<ArchetypeInfo> getOrRegisterArchetypeInfo(const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        assert(TypeUtil::IsFrontType(boost::hana::type_c<Entity>, sanitized_type_list));

        ArchetypeNumber archetype_number = ArchetypeIDGenerator<decltype(sanitized_type_list)>::number();
        if (m_archetype_infos.Has(archetype_number)) {
            return m_archetype_infos[archetype_number];
        } else {
            return registerArchetypeInfo(archetype_number, sanitized_type_list, types_ref);
        }
    }

    static inline WorldNumber s_world_number_counter = 0;

    WorldNumber m_number = 0;

    /// \brief CdOrEntity->所属Archetype検索用の参照
    using ArchetypeMap = SparseSet<RefPtr<ArchetypeInfo>>;//[ArchetypeNumber]
    std::unordered_map<TypeDataID,ArchetypeMap> m_component_index; // [TypeInfo::GetID()]

    /// \brief CD情報の実体 [CdNumber]
    SparseSet<OwnerPtr<TypeInfo>> m_cd_infos;

    /// \brief Archetype情報の実体 [ArchetypeNumber]
    SparseSet<OwnerPtr<ArchetypeInfo>> m_archetype_infos; 

    /// \brief Entityから持っているCDを探す用の参照
    /// \note EntityにArchetypeNUmberを持たせているため、おそらくいらない
    /// TODO:消す
    std::unordered_map<Entity, RefPtr<ArchetypeInfo>> m_entity_datas;

};