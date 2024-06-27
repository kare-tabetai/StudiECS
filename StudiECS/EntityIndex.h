#pragma once
#include "Type.h"
#include "Constant.h"
#include <limits>

struct EntityIndex {

    static constexpr EntityIndex Invalid()
    {
        return EntityIndex();
    }

    auto operator<=>(const EntityIndex&) const = default;

    /// \brief 所属しているchunk内のindex
    uint16 index = kUint16Max;
    /// \brief 所属しているchunkのidnex
    uint16 chunk_index = kUint16Max;
    /// \brief このEntityのArchetypeのArchetypeNumber
    uint32 archetype_number = kUint32Max;
};
