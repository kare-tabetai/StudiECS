#pragma once
#include "Type.h"
#include "ArchetypeBrunch.h"
#include "Chunk.h"
#include <unordered_map>

class ArchetypeInfo {
    ArchetypeInfo(
        ArcheTypeID _id, 
        const Archetype& _archetype, 
        TypeInfoRefContainer&& _type_infos_ref)
        : id(_id)
        , archetype(_archetype)
        , type_infos_ref(_type_infos_ref)
        , chunks()
    {
    }

private:
    void AddChunk() {

    }


    ArcheTypeID id;
    Archetype archetype;
    TypeInfoRefContainer type_infos_ref;//indexはarchetypeのindex
    std::list<Chunk> chunks;
    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> brunch;
};