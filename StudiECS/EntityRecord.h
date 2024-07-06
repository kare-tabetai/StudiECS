#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

union EntityRecord {
    struct Record {
        Record() = default;
        Record(
            uint32 _record_index)
            : archetype_ref(nullptr)
            , record_index(_record_index)
            , generation(0)
            , chunk_index(kUint8Max)
        {}

        RefPtr<ArchetypeInfo> archetype_ref = nullptr;
        RecordIndex record_index = kInvalidRecordIndex;
        Generation generation = kUint16Max;
        ChunkIndex chunk_index = kUint8Max;
    };
    struct DestroyedRecord {
        std::array<std::byte, sizeof(RefPtr<ArchetypeInfo>)> padding0;
        RecordIndex destroyed_index = kUint32Max;
        Generation generation = kUint16Max;
        uint8 destroyed = kUint8Max;
    };

    EntityRecord() = default;
    ~EntityRecord() = default;

    EntityRecord(
        RecordIndex _record_index)
        : record(
            _record_index)
    {
    }

    void Initialize(
        const RefPtr<ArchetypeInfo>& archetype_ref,
        ChunkIndex chunk_index)
    {
        record.archetype_ref = archetype_ref;
        record.chunk_index = chunk_index;
    }
    
    Record record = {};
    DestroyedRecord destroyed_record;
};