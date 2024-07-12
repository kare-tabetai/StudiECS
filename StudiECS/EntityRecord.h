#pragma once
#include "ArchetypeInfo.h"
#include "Chunk.h"
#include "Constant.h"
#include "Type.h"

struct EntityRecord {
    void Initialize(
        const RefPtr<ArchetypeInfo>& _archetype_ref,
         EntityIndex entity_index)
    {
        archetype_ref = _archetype_ref;
        index = entity_index;
    }

    /// \retval çÌèúÇ≥ÇÍÇΩEntityà»ç~ÇÃindexÇÃEntity
    std::vector<ArrayView<Entity>> Destroy(RecordIndex _destroyed_index)
    {
        auto shift_entities = archetype_ref->DestroyEntity(index);
        archetype_ref = nullptr;
        destroyed_index = _destroyed_index;
        incrementGeneration();
        return shift_entities;
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
        return generation;
    }
    RefPtr<ArchetypeInfo> GetArchetypeInfo()
    {
        return archetype_ref;
    }
    bool IsCurrentGeneration(Generation _generation) const
    {
        assert(_generation <= generation);
        return generation == _generation;
    }
    void ReUse(EntityIndex _index)
    {
        index = _index;
    }

private:
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
        EntityIndex index = 0; // ChunksÇÃindex
        RecordIndex destroyed_index;
    };
};