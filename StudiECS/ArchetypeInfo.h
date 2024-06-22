#pragma once
#include "Type.h"
#include <unordered_map>
#include "ArchetypeBrunch.h"
#include "ChunkContainer.h"

struct ArchetypeInfo {
    ArcheTypeID id;
    Archetype cds_id;
    ChunkContainer chunks;
    ///  \brief CD’Ç‰Á‚ÉˆÚ“®‚·‚éArchetype‚Ö‚ÌQÆ
    std::unordered_map<CdID, ArchetypeBrunch> brunch;
};