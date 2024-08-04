#pragma once
#include "ArchetypeInfo.h"
#include "Concept.h"
#include "Constant.h"
#include "Entity.h"
#include "EntityRecord.h"
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
        constexpr auto sanitized = Util::SanitizeTypeList(type_list);

        // m_cd_infosにCDを設定
        TypeInfoRefContainer type_info_refs = registerTypeInfos(sanitized);

        // m_archetype_infosにアーキタイプを設定
        RefPtr<ArchetypeInfo> archetype_ref = getOrRegisterArchetypeInfo(sanitized, type_info_refs);

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
            m_component_index[type_info->GetID()][archetype_ref->GetNumber()] = archetype_ref;
        }

        return entity;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity.GetRecordIndex() < m_entity_record.size());
        if (!IsValid(entity)) {
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
        assert(entity.GetRecordIndex() < m_entity_record.size());
        auto& record = m_entity_record[entity.GetRecordIndex()];
        return record.GetArchetypeInfo()->GetTypes<CD...>(record.GetEntityIndex());
    }

    template<CdOrEntityConcept CdOrEntity>
    std::vector<ArrayView<CdOrEntity>> GetCdArray()
    {
        TypeDataID id = TypeIDGenerator<CdOrEntity>::id();
        auto component_data_itr = m_component_index.find(id);
        if (component_data_itr == m_component_index.end()) {
            return std::vector<ArrayView<CdOrEntity>>();
        }

        auto archetypes = component_data_itr->second.GetArray();
        std::vector<ArrayView<CdOrEntity>> result;
        result.reserve(archetypes.size() * 2);
        for (auto& archetype : archetypes) {
            auto cd_arrays = archetype->GetTypeArrays<CdOrEntity>();
            result.insert(result.end(), cd_arrays.begin(), cd_arrays.end());
        }
        return result;
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
        auto&& info = std::make_unique<ArchetypeInfo>(archetype_number, arche_type, types_ref, m_number);
        m_archetype_infos[archetype_number] = std::move(info);
        return m_archetype_infos[archetype_number].get();
    }

    template<CdOrEntityConcept... T>
    RefPtr<ArchetypeInfo> getOrRegisterArchetypeInfo(const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        assert(TypeUtil::IsFrontType(boost::hana::type_c<Entity>, sanitized_type_list));

        ArchetypeNumber archetype_number = ArchetypeIDGenerator<decltype(sanitized_type_list)>::number();
        if (m_archetype_infos.Has(archetype_number)) {
            return m_archetype_infos[archetype_number].get();
        } else {
            return registerArchetypeInfo(archetype_number, sanitized_type_list, types_ref);
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

    static inline std::atomic<WorldNumber> s_world_number_counter = 0;

    WorldNumber m_number = 0;

    /// \brief CdOrEntity->所属Archetype検索用の参照
    using ArchetypeMap = SparseSet<RefPtr<ArchetypeInfo>>; //[ArchetypeNumber]
    std::unordered_map<TypeDataID, ArchetypeMap> m_component_index; // [TypeInfo::GetID()]

    /// \brief CD情報の実体 [CdNumber]
    SparseSet<OwnerPtr<TypeInfo>> m_cd_infos;

    /// \brief Archetype情報の実体 [ArchetypeNumber]
    SparseSet<OwnerPtr<ArchetypeInfo>> m_archetype_infos;

    /// \brief Entityの情報コンテナ
    /// \note Entity->Chunkとそのindexをたどるよう
    std::vector<EntityRecord> m_entity_record;

    /// \brief m_entity_recordの空要素をたどるよう
    RecordIndex m_destroyed_index = kInvalidRecordIndex;
};