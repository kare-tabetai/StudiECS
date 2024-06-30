#pragma once
#include "ArchetypeInfo.h"
#include "Entity.h"
#include "Type.h"
#include "TypeUtil.h"
#include "SparseSet.h"
#include "Constant.h"
#include "Concept.h"
#include <memory>
#include <unordered_map>

/// \brief ���root�ɂȂ�N���X��������S�����삷��
/// \note �^ID��hash�ł͂Ȃ��ĒP���ȉ��Z�l�ɂ����ق���std::vector�ɂł��đ������ȋC������
class World {
public:
    World() {
        m_number = s_counter;
        s_counter++;
        assert(s_counter != kUint8Max);
    }

    template<CdConcept... Args>
    Entity CreateEntity()
    {
        constexpr auto type_list = TypeUtil::MakeTypeList<Args...>();
        constexpr auto sanitized = Util::SanitizeTypeList(type_list);

        //m_cd_infos��CD��ݒ�
        TypeInfoRefContainer type_info_refs = registerTypeInfos(sanitized);

        //m_archetype_infos�ɃA�[�L�^�C�v��ݒ�
        RefPtr<ArchetypeInfo> archetype_ref = getOrRegisterArchetypeInfo(sanitized, type_info_refs);
        Entity entity = archetype_ref->CreateEntity();

        //m_entity_datas�ɃA�[�L�^�C�v��ݒ�
        assert(m_entity_datas.count(entity) == 0);
        m_entity_datas.emplace(entity, archetype_ref);

        // m_component_index�ɃA�[�L�^�C�v��ݒ�
        for (const auto& type_info : type_info_refs) {
            if (m_component_index.count(type_info->GetID())== 0) {
                m_component_index.emplace(type_info->GetID() ,ArchetypeMap());
            }
            m_component_index[type_info->GetID()][archetype_ref->GetNumber()] = archetype_ref;

        }
        return entity;
    }

    template<CdConcept CD>
    CD* Get(Entity entity) {
        //TODO:
    }

    template<CdConcept... Args>
    std::tuple<Args*...> Get(Entity entity)
    {
        // TODO:
    }

private:

    template<CdOrEntityConcept CD>
    RefPtr<TypeInfo> getOrRegisterTypeInfo()
    {
        CdNumber cd_number = CdIdGenerator<CD>::number();
        if (m_cd_infos.Has(cd_number)) {
            return m_cd_infos[cd_number];
        } else {
            auto&& type_info_ptr = std::make_shared<TypeInfo>(TypeInfo::Make<CD>());
            RefPtr<TypeInfo> ret_ptr = type_info_ptr;
            m_cd_infos[cd_number] = std::move(type_info_ptr);
            return ret_ptr;
        }
    }

    template<CdOrEntityConcept... T>
    TypeInfoRefContainer registerTypeInfos(const hana_tuple<T...>& sanitized_type_list)
    {
        TypeInfoRefContainer refs;
        boost::hana::for_each(sanitized_type_list, [this, &refs](auto t) {
            using T = typename decltype(t)::type;
            auto weak_tpr = getOrRegisterTypeInfo<T>();
            refs.push_back(weak_tpr);
        });
        return refs;
    }

    template<CdOrEntityConcept... T>
    RefPtr<ArchetypeInfo> registerArchetypeInfo(ArchetypeNumber archetype_number, const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        Archetype arche_type = Util::TypeListToArchetype(sanitized_type_list);
        auto&& info = std::make_shared<ArchetypeInfo>(archetype_number, arche_type, types_ref, m_number);
        m_archetype_infos[archetype_number] = std::move(info);
        return m_archetype_infos[archetype_number];
    }

    template<CdOrEntityConcept... T>
    RefPtr<ArchetypeInfo> getOrRegisterArchetypeInfo(const hana_tuple<T...>& sanitized_type_list, const TypeInfoRefContainer& types_ref)
    {
        assert(TypeUtil::IsFrontType(boost::hana::type_c<Entity>, sanitized_type_list));

        ArchetypeNumber archetype_number = ArchetypeIDGenerator<decltype(sanitized_type_list)>::number();
        if (m_archetype_infos.Has(archetype_number)) {
            return m_archetype_infos[archetype_number];
        } else {
            return registerArchetypeInfo(archetype_number, sanitized_type_list, types_ref);
        }
    }

    static inline uint8 s_counter = 0;

    WorldNumber m_number = 0;

    // \brief CdOrEntity->����Archetype�����p�̎Q��
    using ArchetypeMap = SparseSet<RefPtr<ArchetypeInfo>>;//[ArchetypeNumber]
    std::unordered_map<TypeDataID,ArchetypeMap> m_component_index; // [TypeInfo::GetID()]

    // \brief CD���̎��� [CdNumber]
    SparseSet<OwnerPtr<TypeInfo>> m_cd_infos;

    // \brief Archetype���̎��� [ArchetypeNumber]
    SparseSet<OwnerPtr<ArchetypeInfo>> m_archetype_infos; 

    // \brief Entity���玝���Ă���CD��T���p�̎Q��
    std::unordered_map<Entity, RefPtr<ArchetypeInfo>> m_entity_datas;

};