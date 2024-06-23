#pragma once
#include "ArchetypeInfo.h"
#include "Entity.h"
#include "Type.h"
#include "TypeUtil.h"
#include <memory>
#include <unordered_map>

/// \brief 一番rootになるクラスここから全部操作する
/// \note 型IDをhashではなくて単純な加算値にしたほうがstd::vectorにできて早そうな気がする
class World {
public:
    template<class... Args>
    Entity CreateEntity() {
        auto type_list = TypeUtil::MakeTypeList<Args...>();
        auto sanitized = TypeUtil::SanitizeTypeList(type_list);

        return Entity::Invalid();
    }

private:
    std::unordered_map<ArcheTypeID, std::shared_ptr<ArchetypeInfo>> archetype_infos;
    std::unordered_map<Entity, std::weak_ptr<ArchetypeInfo>> entity_to_archetype;

    using ArchetypeMap = std::unordered_map<ArcheTypeID, std::weak_ptr<ArchetypeInfo>>;
    std::unordered_map<CdID, ArchetypeMap> component_index;

    ArchetypeInfo& getOrRegisterArchetypeInfo(const auto& sanitized_type_list)
    {
        ArcheTypeID arch_id = CdIdGenerator<decltype(sanitized_type_list)>::id();
        auto itr = archetype_infos.find(arch_id);
        if (itr != archetype_infos.end()) {
            return itr;
        } else {
            return registerArchetypeInfo(arch_id, sanitized_type_list);
        }
    }

    ArchetypeInfo& registerArchetypeInfo(ArcheTypeID arch_id, const auto& sanitized_type_list)
    {
        auto archetype = Util::TypeListToArchetype(sanitized_type_list);


        auto info = std::make_shared<ArchetypeInfo>(arch_id, std::move(archetype));


        archetype_infos.try_emplace(arch_id, std::move(info));
    }
};