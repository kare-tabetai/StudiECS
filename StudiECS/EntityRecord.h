#pragma once
#include "ArchetypeInfo.h"
#include "Chunk.h"
#include "Constant.h"
#include "Type.h"

struct EntityRecord {
    void Initialize(
        const RefPtr<ArchetypeInfo>& _archetype_ref,
        const EntityIndex& _entity_index)
    {
        local_index = _entity_index.GetLocalIndex();
        archetype_ref = _archetype_ref;
        chunk_index = _entity_index.GetChunkIndex();
    }

    /// \retval çÌèúÇ≥ÇÍÇΩEntityà»ç~ÇÃindexÇÃEntity
    std::vector<ArrayView<Entity>> Destroy(RecordIndex _destroyed_index)
    {
        assert(!isDestroyed());

        auto shift_entities = archetype_ref->DestroyEntity(EntityIndex(chunk_index, local_index));
        archetype_ref = nullptr;
        destroyed_index = _destroyed_index;
        incrementGeneration();
        destroyed = kUint8Max;
        return shift_entities;
    }

    EntityIndex GetEntityIndex() const
    {
        assert(!isDestroyed());
        return EntityIndex(chunk_index,local_index);
    }
    RecordIndex GetDestroyedIndex() const
    {
        assert(isDestroyed());
        return destroyed_index;
    }
    Generation GetGeneration() const
    {
        return generation;
    }
    RefPtr<ArchetypeInfo> GetArchetypeInfo()
    {
        assert(!isDestroyed());
        return archetype_ref;
    }
    bool IsCurrentGeneration(Generation _generation) const
    {
        assert(_generation <= generation);
        return generation == _generation;
    }
    void ReUse(RecordIndex _record_index)
    {
        local_index = _record_index;
    }

private:
    bool isDestroyed() const
    {
        return destroyed == kUint8Max;
    }
    bool incrementGeneration()
    {
        if (generation == kUint16Max) [[unlikely]] {
            assert(false);
            generation = 0;
            return false;
        }
        generation++;
        return true;
    }

    RefPtr<ArchetypeInfo> archetype_ref = nullptr;
    Generation generation = 0;
    union {
        LocalIndex local_index = kInvalidRecordIndex;//TODO:chunk_indexÇ∆Ç‹Ç∆ÇﬂÇƒEntityIndexâª
        RecordIndex destroyed_index;
    };
    union {
        ChunkIndex chunk_index = kUint8Max;
        uint8 destroyed;
    };
};