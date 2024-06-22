#pragma once
#include "Type.h"
#include "ArchetypeInfo.h"
#include <unordered_map>
#include <memory>
#include "Entity.h"


/// \brief ���root�ɂȂ�N���X��������S�����삷��
/// \note �^ID��hash�ł͂Ȃ��ĒP���ȉ��Z�l�ɂ����ق���std::vector�ɂł��đ������ȋC������
class World {
    std::unordered_map<ArcheTypeID, std::shared_ptr<ArchetypeInfo>> archetype_infos;
    std::unordered_map<Entity, std::weak_ptr<ArchetypeInfo>> entity_to_archetype;

    using ArchetypeMap = std::unordered_map<ArcheTypeID, std::weak_ptr<ArchetypeInfo>>;
    std::unordered_map<CdID, ArchetypeMap> component_index;
};