#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

union EntityRecord {
    struct Record {
        RefPtr<ArchetypeInfo> archetype_ref = nullptr;
        uint32 index = kUint32Max;
        Generation generation = kUint16Max;
        ChunkIndex chunk_index = kUint8Max;
    };
    struct DestroyedRecord {
        std::array<std::byte, sizeof(RefPtr<ArchetypeInfo>)> padding0;
        RecordIndex destroyed_index = kUint32Max;
        Generation generation = kUint16Max;
        uint8 destroyed = kUint8Max;
    };
    
    Record record = {};
    DestroyedRecord destroyed_record;
};