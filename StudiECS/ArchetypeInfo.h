#pragma once
#include "ArchetypeBrunch.h"
#include "Chunk.h"
#include "OffsetArrayView.h"
#include "Type.h"
#include "Entity.h"
#include "ContainerTypes.h"
#include <unordered_map>

class ArchetypeInfo {
public:
    ArchetypeInfo(
        ArchetypeNumber _number,
        const Archetype& _archetype,
        const TypeInfoRefContainer& _type_infos)
        : m_number(_number)
        , m_archetype(_archetype)
        , m_type_infos(_type_infos)
        , m_max_chunk_entity_max(Chunk::CalcMaxEntityCount(_type_infos))
        , m_chunks()
        , m_brunch()
    {
        addChunk();
    }

    Entity CreateEntity()
    {
        if (m_released_entity_index != EntityIndex::Invalid()) {
            auto* entity_ptr = getEntity(m_released_entity_index);
            m_released_entity_index = entity_ptr->Index();
        }
        return Entity::Invalid();
    }

private:
    void addChunk()
    {
        auto chunk = std::make_shared<Chunk>(m_type_infos, m_max_chunk_entity_max);
        m_chunks.emplace_back(std::move(chunk));
    }

    void construct(EntityIndex entity_index)
    {
        assert(entity_index.chunk_index < m_chunks.size());
        auto& chunk = m_chunks[entity_index.chunk_index];

        // MEMO: Entityは初期化しない
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(cd_index, entity_index.index, m_type_infos[cd_index]->GetTypeSize());
            m_type_infos[cd_index]->Construct(cd);
        }
    }

    Entity* getEntity(EntityIndex entity_index)
    {
        assert(entity_index.chunk_index < m_chunks.size());
        auto& chunk = m_chunks[entity_index.chunk_index];
        return chunk->GetEntity(entity_index.index);
    }

    ArchetypeNumber m_number;
    Archetype m_archetype;

    /// \brief 空きentityへのindex
    EntityIndex m_released_entity_index; 

    /// \brief 型情報への参照 indexはcd_index
    TypeInfoRefContainer m_type_infos;

    uint32 m_max_chunk_entity_max = 0;
    std::vector<OwnerPtr<Chunk>> m_chunks;

    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> m_brunch;
};