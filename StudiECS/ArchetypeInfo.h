#pragma once
#include "ArchetypeBrunch.h"
#include "Chunk.h"
#include "OffsetArrayView.h"
#include "Type.h"
#include <unordered_map>

class ArchetypeInfo {
    ArchetypeInfo(
        ArcheTypeID _id,
        const Archetype& _archetype,
        const TypeInfoRefContainer& _type_infos)
        : m_id(_id)
        , m_archetype(_archetype)
        , m_type_infos(_type_infos)
        , m_max_chunk_entity_max(Chunk::CalcMaxEntityCount(_type_infos))
        , m_chunks()
        , m_brunch()
    {
        addChunk();
    }

private:
    void addChunk()
    {
        m_chunks.emplace_back(m_type_infos, m_max_chunk_entity_max);
    }

    ArcheTypeID m_id;
    Archetype m_archetype;
    TypeInfoRefContainer m_type_infos;
    uint32 m_max_chunk_entity_max = 0;
    std::list<Chunk> m_chunks;
    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> m_brunch;
};