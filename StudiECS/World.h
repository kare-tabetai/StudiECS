#pragma once
#include "ArchetypeInfo.h"
#include "Concept.h"
#include "Constant.h"
#include "Entity.h"
#include "EntityRecord.h"
#include "CdIdGenerator.h"
#include "SparseSet.h"
#include "Type.h"
#include "TypeUtil.h"
#include <memory>
#include <unordered_map>
#include <vector>

/// \brief 一番rootになるクラスここから全部操作する
class World {
public:
    World()
    {
        m_number = s_world_number_counter;
        if (s_world_number_counter != kUint8Max) {
            s_world_number_counter++;
        }
        assert(s_world_number_counter != kUint8Max);
    }

    template<CdConcept... Args>
    [[nodiscard]] Entity CreateEntity()
    {
        constexpr auto type_list = TypeUtil::MakeTypeList<Args...>();
        constexpr auto sanitized_type_list = Util::SanitizeTypeList(type_list);

        // m_cd_infosにCDを設定
        TypeInfoRefContainer type_info_refs = getAndRegisterTypeInfos(sanitized_type_list);
        // m_archetype_infosにアーキタイプを設定
        Archetype arche_type = Util::TypeListToArchetype(sanitized_type_list);
        sortTypeInfosRefsAndArchetype(type_info_refs, arche_type);

        RefPtr<ArchetypeInfo> archetype_ref = getOrRegisterArchetypeInfo(arche_type, type_info_refs);

        RecordIndex record_index = getOrCreateEntityRecord();
        assert(record_index < m_entity_record.size());

        auto [entity, entity_index] = archetype_ref->CreateEntity(
            record_index,
            m_entity_record[record_index].GetGeneration());

        m_entity_record[record_index].Initialize(archetype_ref, entity_index);

        // m_component_indexにアーキタイプを設定
        for (const auto& type_info : type_info_refs) {
            if (m_component_index.count(type_info->GetID()) == 0) {
                m_component_index.emplace(type_info->GetID(), ArchetypeMap());
            }
            auto& archetype_map = m_component_index[type_info->GetID()];
            auto archetype_map_itr = std::find(archetype_map.begin(), archetype_map.end() ,archetype_ref);
            if (archetype_map_itr == archetype_map.end()) {
                archetype_map.emplace_back(archetype_ref);
            }
        }

        return entity;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity.GetRecordIndex() < m_entity_record.size());
        if (!IsValid(entity)) [[unlikely]] {
            return;
        }

        auto& destroy_record = m_entity_record[entity.GetRecordIndex()];
        auto shift_entities = destroy_record.Destroy(m_destroyed_index);
        m_destroyed_index = entity.GetRecordIndex();
        assert(!IsValid(entity));

        for (auto& shift_entity_array : shift_entities) {
            for (Entity shift_entity : shift_entity_array) {
                auto& shift_entity_record = m_entity_record[shift_entity.GetRecordIndex()];
                shift_entity_record.DecrementIndex();
                assert(IsValid(shift_entity));
            }
        }

#if defined(_DEBUG)
        for (auto& shift_entity_array : shift_entities) {
            for (Entity shift_entity : shift_entity_array) {
                auto& assert_entity_record = m_entity_record[shift_entity.GetRecordIndex()];
                auto assert_archetype_info = assert_entity_record.GetArchetypeInfo();
                auto assert_entity = assert_archetype_info->GetCD<Entity>(assert_entity_record.GetEntityIndex());
                assert(assert_entity->GetRecordIndex() == shift_entity.GetRecordIndex());
            }
        }
#endif
    }

    bool IsValid(Entity entity) const
    {
        if (m_entity_record.size() <= entity.GetRecordIndex()) {
            return false;
        }

        const auto& record = m_entity_record[entity.GetRecordIndex()];
        if (!record.IsValid()) {
            return false;
        }
        if (!record.IsCurrentGeneration(entity.GetGeneration())) {
            return false;
        }

        const auto archetype_info = record.GetArchetypeInfo();
        const auto* entity_ptr = archetype_info->GetCD<Entity>(record.GetEntityIndex());
        return !entity_ptr->IsInvalid();
    }

    template<CdConcept CD>
    CD* Get(Entity entity)
    {
        PtrTuple<CD> cd_ptr_tuple = GetTypes<CD>(entity);
        return std::get<0>(cd_ptr_tuple);
    }

    template<CdConcept... CD>
    PtrTuple<CD...> GetTypes(Entity entity)
    {
        // TODO: nullptrのtupleを返すようにする
        assert(IsValid(entity));

        assert(entity.GetRecordIndex() < m_entity_record.size());
        auto& record = m_entity_record[entity.GetRecordIndex()];
        return record.GetArchetypeInfo()->GetTypes<CD...>(record.GetEntityIndex());
    }

    template<CdOrEntityConcept CdOrEntity>
    std::vector<ArrayView<CdOrEntity>> GetCdArray()
    {
        constexpr TypeDataID kTypeDataID = TypeIDGenerator<CdOrEntity>::id();
        auto component_data_itr = m_component_index.find(kTypeDataID);
        if (component_data_itr == m_component_index.end()) {
            return std::vector<ArrayView<CdOrEntity>>();
        }

        std::vector<ArrayView<CdOrEntity>> result;
        result.reserve(component_data_itr->second.size() * 2);
        for (auto& archetype : component_data_itr->second) {
            auto cd_arrays = archetype->GetTypeArrays<CdOrEntity>();
            result.insert(result.end(), cd_arrays.begin(), cd_arrays.end());
        }
        return result;
    }

    /// \brief 既存のentityにCDを追加する
    /// \note 関数呼び出し前にこのentityへのGetCD、entityが含んでいる型のGetCdArrayで取得した参照情報は
    /// 無効(未定義動作)になる
    template<CdConcept CD>
    CD* AddCD(Entity entity)
    {
        if (!IsValid(entity)) [[unlikely]] {
            return nullptr;
        }

        constexpr CdID kCdTypeID = TypeIDGenerator<CD>::id();

        auto& record = m_entity_record[entity.GetRecordIndex()];
        auto archetype_ref = record.GetArchetypeInfo();

        // すでに同じCDがついていた場合は返す
        if (archetype_ref->Has<CD>()) [[unlikely]] {
            return nullptr;
        }

        // キャッシュが取得できればそれを使う
        auto add_archetype_info = archetype_ref->TryGetAddCdArchetypeInfo(kCdTypeID);
        // キャッシュが取得できない場合はArchetypeInfoを検索
        if (!add_archetype_info) {
            auto new_archetype = archetype_ref->GetArcehtype();
            new_archetype.push_back(kCdTypeID);
            sortArchetype(new_archetype);

            auto itr = std::find_if(
                m_archetype_infos.begin(),
                m_archetype_infos.end(),
                [&new_archetype](const OwnerPtr<ArchetypeInfo>& item) {
                    return item->IsSameArchetype(new_archetype);
                });

            // ArchetypeInfoが見つからなければ新たに登録
            if (itr == m_archetype_infos.end()) {
                auto new_type_info_ref_container = archetype_ref->GetTypeInfoRefContainer();
                new_type_info_ref_container.push_back(getOrRegisterTypeInfo<CD>());
                sortTypeInfosRefsAndArchetype(new_type_info_ref_container, new_archetype);
                add_archetype_info = getOrRegisterArchetypeInfo(new_archetype, new_type_info_ref_container);
            } else {
                add_archetype_info = itr->get();
            }

            archetype_ref->RegisterAddCdArchetypeInfo<CD>( add_archetype_info);
        }

        assert(add_archetype_info);

        // Entityの内容を新しいArchetypeInfoへ移動してEntityRecordのIndexも更新
        record.ChangeArchetype(entity.GetRecordIndex(), add_archetype_info);

        // OPTIMIZE:ChangeAddArchetypeの処理中に取得したほうが処理が軽そう
        return Get<CD>(entity);
    }

    template<CdConcept CD>
    void RemoveCD(Entity entity) {
        if (!IsValid(entity)) [[unlikely]] {
            return ;
        }

        constexpr CdID kCdTypeID = TypeIDGenerator<CD>::id();

        auto& record = m_entity_record[entity.GetRecordIndex()];
        auto archetype_ref = record.GetArchetypeInfo();

        // 該当のCDがついていなければ返す
        if (!archetype_ref->Has<CD>()) [[unlikely]] {
            return;
        }

        // キャッシュが取得できればそれを使う
        auto remove_archetype_info = archetype_ref->TryGetRemoveCdArchetypeInfo(kCdTypeID);
        // キャッシュが取得できない場合はArchetypeInfoを検索
        if (!remove_archetype_info) {
            auto new_archetype = archetype_ref->GetArcehtype();
            [[maybe_unused]] auto archetype_erase_ret = std::erase(new_archetype, kCdTypeID);
            assert(archetype_erase_ret == 1);

            auto itr = std::find_if(
                m_archetype_infos.begin(),
                m_archetype_infos.end(),
                [&new_archetype](const OwnerPtr<ArchetypeInfo>& item) {
                    return item->IsSameArchetype(new_archetype);
                });

            // ArchetypeInfoが見つからなければ新たに登録
            if (itr == m_archetype_infos.end()) {
                auto new_type_info_ref_container = archetype_ref->GetTypeInfoRefContainer();
                [[maybe_unused]] auto type_info_ref_erase_ret = std::erase(new_type_info_ref_container, getOrRegisterTypeInfo<CD>());
                assert(type_info_ref_erase_ret == 1);

                remove_archetype_info = getOrRegisterArchetypeInfo(new_archetype, new_type_info_ref_container);
            } else {
                remove_archetype_info = itr->get();
            }

            archetype_ref->RegisterRemoveCdArchetypeInfo<CD>(remove_archetype_info);
        }

        assert(remove_archetype_info);

        // Entityの内容を新しいArchetypeInfoへ移動してEntityRecordのIndexも更新
        record.ChangeArchetype(entity.GetRecordIndex(), remove_archetype_info);
    }

private:

    template<CdOrEntityConcept CD>
    RefPtr<TypeInfo> getOrRegisterTypeInfo()
    {
        CdNumber cd_number = CdIdGenerator<CD>::number();
        if (m_cd_infos.Has(cd_number)) {
            return m_cd_infos[cd_number].get();
        } else {
            auto&& type_info_ptr = std::make_unique<TypeInfo>(TypeInfo::Make<CD>());
            RefPtr<TypeInfo> ret_ptr = type_info_ptr.get();
            m_cd_infos[cd_number] = std::move(type_info_ptr);
            return ret_ptr;
        }
    }

    template<CdOrEntityConcept... T>
    TypeInfoRefContainer getAndRegisterTypeInfos(const hana_tuple<T...>& sanitized_type_list)
    {
        TypeInfoRefContainer refs;
        boost::hana::for_each(sanitized_type_list, [this, &refs](auto t) {
            using T = typename decltype(t)::type;
            auto weak_tpr = getOrRegisterTypeInfo<T>();
            refs.push_back(weak_tpr);
        });
        return refs;
    }

    RefPtr<ArchetypeInfo> getOrRegisterArchetypeInfo(const Archetype& arche_type, const TypeInfoRefContainer& types_ref)
    {
        assert(types_ref.front()->GetID() == TypeIDGenerator<Entity>::id());
        assert(arche_type.front() == TypeIDGenerator<Entity>::id());

        auto archetype_infos_itr = std::find_if(
            m_archetype_infos.begin(), 
            m_archetype_infos.end(),
            [&arche_type](const OwnerPtr<ArchetypeInfo>& item) {
                return item->IsSameArchetype(arche_type);
            });
        if (archetype_infos_itr == m_archetype_infos.end()) {
            auto&& info = std::make_unique<ArchetypeInfo>(arche_type, types_ref, m_number);
            m_archetype_infos.push_back(std::move(info));
            return m_archetype_infos.back().get();
        } else {
            return archetype_infos_itr->get();
        }
    }

    RecordIndex getOrCreateEntityRecord()
    {
        if (m_destroyed_index == kInvalidRecordIndex) {
            // MEMO:追加する最後の要素のindexだからsizeでいい
            RecordIndex record_index = static_cast<RecordIndex>(m_entity_record.size());
            m_entity_record.emplace_back();
            return record_index;
        } else {
            RecordIndex record_index = m_destroyed_index;
            m_destroyed_index = m_entity_record[record_index].GetDestroyedIndex();
            m_entity_record[record_index].ReUse(record_index);
            return record_index;
        }
    }

    /// \brief Archetypeのsortを行う
    void sortArchetype(Archetype& arche_type)
    {
        std::sort(
            arche_type.begin(),
            arche_type.end(),
            [](CdID lhs, CdID rhs) {
                // MEMO:Entityは先頭になるようにソート
                if (lhs == TypeIDGenerator<Entity>::id()) {
                    return true;
                } else if (rhs == TypeIDGenerator<Entity>::id()) {
                    return false;
                } else {
                    return lhs < rhs;
                }
            });

        assert(arche_type.front() == TypeIDGenerator<Entity>::id());
    }

    void sortTypeInfoRefs(TypeInfoRefContainer& type_info_refs)
    {
        std::sort(
            type_info_refs.begin(),
            type_info_refs.end(),
            [](const RefPtr<TypeInfo>& lhs, const RefPtr<TypeInfo>& rhs) {
                if (lhs->GetID() == TypeIDGenerator<Entity>::id()) {
                    return true;
                } else if (rhs->GetID() == TypeIDGenerator<Entity>::id()) {
                    return false;
                } else {
                    return lhs->GetID() < rhs->GetID();
                }
            });

        assert(type_info_refs.front()->GetID() == TypeIDGenerator<Entity>::id());
    }

    void sortTypeInfosRefsAndArchetype(TypeInfoRefContainer& type_info_refs, Archetype& arche_type) {
        assert(type_info_refs.size() == arche_type.size());
        
        sortArchetype(arche_type);
        sortTypeInfoRefs(type_info_refs);

#if defined(_DEBUG)
        for (size_t i = 0; i < arche_type.size(); i++) {
            assert(arche_type[i] == type_info_refs[i]->GetID());
        }
#endif
    }

    static inline std::atomic<WorldNumber> s_world_number_counter = 0;

    WorldNumber m_number = 0;

    /// \brief CdOrEntity->所属Archetype検索用の参照
    using ArchetypeMap =std::vector<RefPtr<ArchetypeInfo>>;
    std::unordered_map<TypeDataID, ArchetypeMap> m_component_index; // [TypeInfo::GetID()]

    /// \brief CD情報の実体 [CdNumber]
    SparseSet<OwnerPtr<TypeInfo>> m_cd_infos;

    /// \brief Archetype情報の実体 [ArchetypeNumber]
    std::vector<OwnerPtr<ArchetypeInfo>> m_archetype_infos;

    /// \brief Entityの情報コンテナ
    /// \note Entity->Chunkとそのindexをたどるよう
    std::vector<EntityRecord> m_entity_record;

    /// \brief m_entity_recordの空要素をたどるよう
    RecordIndex m_destroyed_index = kInvalidRecordIndex;
};