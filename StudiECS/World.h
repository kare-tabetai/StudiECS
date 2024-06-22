#pragma once
#include "Type.h"
#include "ArchetypeInfo.h"
#include <unordered_map>
#include <memory>
#include "Entity.h"


/// \brief 一番rootになるクラスここから全部操作する
/// \note 型IDをhashではなくて単純な加算値にしたほうがstd::vectorにできて早そうな気がする
class World {
    std::unordered_map<ArcheTypeID, std::shared_ptr<ArchetypeInfo>> archetype_infos;
    std::unordered_map<Entity, std::weak_ptr<ArchetypeInfo>> entity_to_archetype;

    using ArchetypeMap = std::unordered_map<ArcheTypeID, std::weak_ptr<ArchetypeInfo>>;
    std::unordered_map<CdID, ArchetypeMap> component_index;
};