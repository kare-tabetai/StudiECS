#pragma once
#include "Type.h"
#include "Constant.h"
#include "Chunk.h"
#include "ArchetypeInfo.h"

struct EntityRecord {
    uint16 index = kUint16Max;
    RefPtr<Chunk> chunk_ref = nullptr;
    RefPtr<ArchetypeInfo> archetype_ref = nullptr;
};