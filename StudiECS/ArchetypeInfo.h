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
        ArchetypeNumber archetype_number,
        const Archetype& archetype,
        const TypeInfoRefContainer& type_infos,
        WorldNumber world_number)
        : m_world_number(world_number)
        , m_archetype_number(archetype_number)
        , m_archetype(archetype)
        , m_released_entity_index(world_number, archetype_number,0,0)
        , m_type_infos(type_infos)
        , m_max_chunk_entity_max(Chunk::CalcMaxEntityCount(type_infos))
        , m_chunks()
        , m_brunch()
    {
        addChunk();
    }

    Entity CreateEntity()
    {
        assert(m_released_entity_index != EntityIndex::Invalid());
        auto* entity_ptr = getEntity(m_released_entity_index);
        m_released_entity_index = entity_ptr->SwapIndex(m_released_entity_index);
        entity_ptr->RemoveFlag(Entity::Flag::Invalid);//使用中にする

        // Chunkがいっぱいになった場合
        if (m_max_chunk_entity_max <= m_released_entity_index.index) [[unlikely]] {
            m_released_entity_index = addChunk();
        }

        construct(entity_ptr->GetIndex());
        return *entity_ptr;
    }

    ArchetypeNumber GetNumber() const {
        return m_archetype_number;
    }

private:
    /// \ret_val 追加したchunkの最初の要素を示すindexを返す
    EntityIndex addChunk()
    {
        auto chunk = std::make_shared<Chunk>(
            m_type_infos, 
            m_max_chunk_entity_max,
            m_world_number,
            m_archetype_number,
            static_cast<ChunkIndex>(m_chunks.size()));
        m_chunks.emplace_back(std::move(chunk));
        return EntityIndex(m_world_number, m_archetype_number,static_cast<ChunkIndex>( m_chunks.size() - 1), 0);
    }

    void construct(EntityIndex entity_index)
    {
        assert(entity_index.chunk_index < m_chunks.size());
        auto& chunk = m_chunks[entity_index.chunk_index];

        // MEMO: Entityは初期化しない
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index, 
                entity_index.index,
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize())
            );
            m_type_infos[cd_index]->Construct(cd);
        }
    }

    Entity* getEntity(EntityIndex entity_index)
    {
        assert(entity_index.chunk_index < m_chunks.size());
        auto& chunk = m_chunks[entity_index.chunk_index];
        return chunk->GetEntity(entity_index.index);
    }

    WorldNumber m_world_number;
    ArchetypeNumber m_archetype_number;
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