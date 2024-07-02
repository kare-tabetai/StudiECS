#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

struct EntityRecord {
    uint16 index = kUint16Max;
    ChunkIndex chunk_index = kUint8Max;
    RefPtr<ArchetypeInfo> archetype_ref = nullptr;
};