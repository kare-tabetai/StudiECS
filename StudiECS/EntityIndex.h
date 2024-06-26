#pragma once
#include "Type.h"
#include "Constant.h"
#include <limits>

struct EntityIndex {

    static constexpr EntityIndex Invalid()
    {
        return EntityIndex();
    }

    constexpr EntityIndex() = default;

    constexpr EntityIndex(
        WorldNumber _world_number,
        ArchetypeNumber _archetype_number,
        ChunkIndex _chunk_index,
        uint16 _index)
        : index(_index)
        , chunk_index(_chunk_index)
        , archetype_number(_archetype_number)
        , world_number(_world_number) {};

    auto operator<=>(const EntityIndex&) const = default;

    /// \brief 所属しているchunk内のindex
    uint16 index = kUint16Max;
    /// \brief 所属しているchunkのidnex
    ChunkIndex chunk_index = kUint16Max;
    /// \brief このEntityのArchetypeのArchetypeNumber
    ArchetypeNumber archetype_number = kUint32Max;
    /// \brief このEntityが属するWorldのNumber
    WorldNumber world_number = kUint8Max;
};
