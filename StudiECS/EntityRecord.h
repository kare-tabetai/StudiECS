#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

struct EntityRecord {
    void Initialize(
        const RefPtr<ArchetypeInfo>& _archetype_ref,
        uint32 _index,
        ChunkIndex _chunk_index)
    {
        index = _index;
        archetype_ref = _archetype_ref;
        chunk_index = _chunk_index;
    }
    void Destroy(Entity entity,RecordIndex _destroyed_index)
    {
        assert(!isDestroyed());

        archetype_ref->DestroyEntity(entity, chunk_index,index);
        archetype_ref = nullptr;
        destroyed_index = _destroyed_index;
        incrementGeneration();
        destroyed = kUint8Max;
    }

    uint32 GetIndex()const
    {
        assert(!isDestroyed());
        return index; 
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
    ChunkIndex GetChunkIndex()const
    {
        assert(!isDestroyed());
        return chunk_index;
    }
    RecordIndex GetRecordIndex() const
    {
        assert(!isDestroyed());
        return index;
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
        index = _record_index;
    }

private:
    bool isDestroyed() const{
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
    union {
        uint32 index = kInvalidRecordIndex;
        RecordIndex destroyed_index;
    };
    Generation generation = 0;
    union {
        ChunkIndex chunk_index = kUint8Max;
        uint8 destroyed;
    };
};