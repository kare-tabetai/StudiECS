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
    Entity CreateEntity()
    {
        constexpr auto type_list = TypeUtil::MakeTypeList<Args...>();
        constexpr auto sanitized = TypeUtil::SanitizeTypeList(type_list);
        TypeInfoRefContainer type_info_refs = registerTypeInfos(sanitized);

        return Entity::Invalid();
    }

private:
    TypeInfoContainer type_infos;

    std::unordered_map<ArcheTypeID, std::shared_ptr<ArchetypeInfo>> archetype_infos;
    std::unordered_map<Entity, std::weak_ptr<ArchetypeInfo>> entity_to_archetype;

    using ArchetypeMap = std::unordered_map<ArcheTypeID, std::weak_ptr<ArchetypeInfo>>;
    std::unordered_map<CdID, ArchetypeMap> component_index;

    template<typename... T>
    TypeInfoRefContainer registerTypeInfos(const boost::hana::tuple<T...>& sanitized_type_list)
    {
        TypeInfoRefContainer refs;
        boost::hana::for_each(sanitized_type_list, [this,&refs](auto t) {
            using T = typename decltype(t)::type;
            auto weak_tpr = getOrRegisterTypeInfo<T>();
            refs.push_back(weak_tpr);
        });
        return refs;
    }

    template<class CD>
    std::weak_ptr<TypeInfo> getOrRegisterTypeInfo()
    {
        constexpr CdID cd_id = CdIdGenerator<CD>::id();
        auto itr = type_infos.find(cd_id);
        if (itr != type_infos.end()) {
            return itr->second;
        } else {
            auto&& type_info_ptr = std::make_shared<TypeInfo>(TypeInfo::Make<CD>());
            std::weak_ptr<TypeInfo> ret_ptr = type_info_ptr;
            type_infos.try_emplace(cd_id, std::move(type_info_ptr));
            return ret_ptr;
        }
    }

    template<typename... T>
    ArchetypeInfo& getOrRegisterArchetypeInfo(const boost::hana::tuple<T...>& sanitized_type_list)
    {
        constexpr ArcheTypeID arch_id = CdIdGenerator<decltype(sanitized_type_list)>::id();
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