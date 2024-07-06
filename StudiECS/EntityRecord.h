#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

struct EntityRecord {
    EntityRecord(RecordIndex record_index)
        : archetype_ref(nullptr)
        , record_index(record_index)
        , generation(0)
        , chunk_index(kUint8Max)
    {
    }

    ~EntityRecord() = default;

    void Initialize(
        const RefPtr<ArchetypeInfo>& _archetype_ref,
        ChunkIndex _chunk_index)
    {
        archetype_ref = _archetype_ref;
        chunk_index = _chunk_index;
    }

    RecordIndex GetDestroyedIndex() {
        assert(destroyed == kUint8Max);
        return destroyed_index;
    }
    void ReUse(RecordIndex _record_index)
    {
        record_index = _record_index;
    }
    
    RefPtr<ArchetypeInfo> archetype_ref;
    union {
        RecordIndex record_index = kInvalidRecordIndex;
        RecordIndex destroyed_index;
    };
    Generation generation = kUint16Max;
    union {
        ChunkIndex chunk_index = kUint8Max;
        uint8 destroyed;
    };
};