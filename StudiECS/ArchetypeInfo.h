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
        // TODO: ChunkContainer�̏�����
    }

private:
    ArcheTypeID id;
    Archetype archetype;
    ChunkContainer chunks;
    ///  \brief CD�ǉ����Ɉړ�����Archetype�ւ̎Q�ƃL���b�V��
    std::unordered_map<CdID, ArchetypeBrunch> brunch;
};