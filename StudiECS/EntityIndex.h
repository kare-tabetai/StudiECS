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

    /// \brief �������Ă���chunk����index
    uint16 index = kUint16Max;
    /// \brief �������Ă���chunk��idnex
    uint16 chunk_index = kUint16Max;
    /// \brief ����Entity��Archetype��ArchetypeNumber
    uint32 archetype_number = kUint32Max;
};
