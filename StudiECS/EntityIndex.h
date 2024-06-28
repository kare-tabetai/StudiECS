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

    /// \brief �������Ă���chunk����index
    uint16 index = kUint16Max;
    /// \brief �������Ă���chunk��idnex
    ChunkIndex chunk_index = kUint16Max;
    /// \brief ����Entity��Archetype��ArchetypeNumber
    ArchetypeNumber archetype_number = kUint32Max;
    /// \brief ����Entity��������World��Number
    WorldNumber world_number = kUint8Max;
};
