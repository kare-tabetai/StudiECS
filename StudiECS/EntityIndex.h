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

    /// \brief Š‘®‚µ‚Ä‚¢‚échunk“à‚Ìindex
    uint16 index = kUint16Max;
    /// \brief Š‘®‚µ‚Ä‚¢‚échunk‚Ìidnex
    uint16 chunk_index = kUint16Max;
    /// \brief ‚±‚ÌEntity‚ÌArchetype‚ÌArchetypeNumber
    uint32 archetype_number = kUint32Max;
};
