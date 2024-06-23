#pragma once
#include "Type.h"
#include "ArchetypeBrunch.h"
#include "ChunkContainer.h"
#include <unordered_map>

class ArchetypeInfo {
    ArchetypeInfo(ArcheTypeID _id, const Archetype& _archetype)
        : id(_id)
        , archetype(_archetype)
        , chunks()
    {
        // TODO: ChunkContainerの初期化
    }

private:
    ArcheTypeID id;
    Archetype archetype;
    ChunkContainer chunks;
    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> brunch;
};