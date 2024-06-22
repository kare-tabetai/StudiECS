#pragma once
#include "Type.h"
#include <unordered_map>
#include "ArchetypeBrunch.h"
#include "ChunkContainer.h"

struct ArchetypeInfo {
    ArcheTypeID id;
    Archetype cds_id;
    ChunkContainer chunks;
    ///  \brief CD�ǉ����Ɉړ�����Archetype�ւ̎Q��
    std::unordered_map<CdID, ArchetypeBrunch> brunch;
};