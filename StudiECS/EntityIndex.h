#pragma once
#include "Type.h"
#include "Constant.h"
#include <limits>

struct EntityIndex {

    static constexpr EntityIndex Invalid()
    {
        return EntityIndex();
    }

    EntityIndex() = default;

    EntityIndex(
        WorldNumber _world_number,
        ArchetypeNumber _archetype_number,
        ChunkIndex _chunk_index,
        uint16 _index)
        : index(_index)
        , chunk_index(_chunk_index)
        , archetype_number(_archetype_number)
        , world_number(_world_number) {};

    auto operator<=>(const EntityIndex&) const = default;

    /// \brief Š‘®‚µ‚Ä‚¢‚échunk“à‚Ìindex
    uint16 index = kUint16Max;
    /// \brief Š‘®‚µ‚Ä‚¢‚échunk‚Ìidnex
    ChunkIndex chunk_index = kUint16Max;
    /// \brief ‚±‚ÌEntity‚ÌArchetype‚ÌArchetypeNumber
    ArchetypeNumber archetype_number = kUint32Max;
    /// \brief ‚±‚ÌEntity‚ª‘®‚·‚éWorld‚ÌNumber
    WorldNumber world_number = kUint8Max;
};
