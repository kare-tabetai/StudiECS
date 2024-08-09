#pragma once
#include "ArchetypeInfo.h"
#include "Constant.h"
#include "Type.h"

struct EntityRecord {
    void Initialize(
        const RefPtr<ArchetypeInfo>& _archetype_ref,
         EntityIndex entity_index)
    {
        assert(!IsValid());

        m_archetype_ref = _archetype_ref;
        index = entity_index;
    }

    /// \retval 削除されたEntity以降のindexのEntity
    std::vector<ArrayView<Entity>> Destroy(RecordIndex _destroyed_index)
    {
        assert(IsValid());

        auto shift_entities = m_archetype_ref->DestroyEntity(index);
        m_archetype_ref = nullptr;
        destroyed_index = _destroyed_index;
        incrementGeneration();
        return shift_entities;
    }

    void ChangeArchetype(RecordIndex record_index, RefPtr<ArchetypeInfo> new_archetype_info)
    {
        if (!IsValid()) {
            return;
        }

        auto old_archetype_info = m_archetype_ref;
        auto old_cds = old_archetype_info->GetTypesRaw(index);
        auto old_type_ref_container = old_archetype_info->GetTypeInfoRefContainer();

        // 移動後ArchetypeInfoにEntityを作成
        auto [new_entity, new_entity_index] = new_archetype_info->CreateEntity(record_index, m_generation);

        // 移動後のEntityにCDとEntityをコピー
        // MEMO:Entityは移動前の内容と同じでいいためそのままコピー
        auto new_cds = new_archetype_info->GetTypesRaw(new_entity_index);
        for (size_t i = 0; i < old_type_ref_container.size(); i++) {
            auto* source_cd = old_cds[i];
            auto type_info = old_type_ref_container[i];
            auto dest_cd_index = new_archetype_info->GetCdIndexByTypeInfo(type_info);
            
            // コピー先のArchetypeに型が含まれていない場合は移動を行わない
            if (!dest_cd_index.has_value()) {
                continue;
            }

            auto* dest_cd = new_cds[i];
            [[maybe_unused]] auto move_or_copy_success = TypeInfoHelper::MoveOrCopy(*type_info, source_cd, dest_cd);
            assert(move_or_copy_success);

            // 移動orコピー後にデストラクト
            type_info->Destruct(source_cd);
        }

        // EntityRecordの内容を移動後のものに変更
        m_archetype_ref = new_archetype_info;
        index = new_entity_index;
    }

    EntityIndex GetEntityIndex() const
    {
        return index;
    }
    RecordIndex GetDestroyedIndex() const
    {
        return destroyed_index;
    }
    Generation GetGeneration() const
    {
        return m_generation;
    }
    RefPtr<ArchetypeInfo> GetArchetypeInfo()
    {
        return m_archetype_ref;
    }
    const RefPtr<ArchetypeInfo> GetArchetypeInfo()const
    {
        return m_archetype_ref;
    }
    bool IsCurrentGeneration(Generation _generation) const
    {
        assert(_generation <= m_generation);
        return m_generation == _generation;
    }
    void ReUse(EntityIndex _index)
    {
        assert(!IsValid());
        index = _index;
    }
    void DecrementIndex()
    {
        assert(0 < index);
        index--;
    }
    bool IsValid() const { return m_archetype_ref != nullptr; }

private:
    bool incrementGeneration()
    {
        if (m_generation == kUint16Max) [[unlikely]] {
            assert(false);
            m_generation = 0;
            return false;
        }
        m_generation++;
        return true;
    }

    RefPtr<ArchetypeInfo> m_archetype_ref = nullptr;
    Generation m_generation = 0;
    union {
        EntityIndex index = 0; // Chunksのindex
        RecordIndex destroyed_index;
    };
};