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
        : 
#if _DEBUG
        m_archetype_name(calcArchetypeName(type_infos))
        ,
#endif // DEBUG
        m_world_number(world_number)
        , m_archetype_number(archetype_number)
        , m_archetype(archetype)
        , m_type_infos(type_infos)
        , m_max_chunk_entity_max(Chunk::CalcMaxEntityCount(type_infos))
        , m_chunks()
        , m_brunch()
    {
        addChunk();
    }

    std::tuple<Entity, ChunkIndex> CreateEntity(RecordIndex record_index, Generation generation)
    {
        auto chunk_index = static_cast<ChunkIndex>(m_chunks.size() - 1); // 最後のchunk
        auto index = m_empty_index;
        auto* entity_ptr = getEntity(
            chunk_index,
            index);
        assert(entity_ptr->IsInvalid());

        entity_ptr->ReSet(record_index, generation, m_world_number); // 使用中にする

        m_empty_index++; // 空きentityを指すように加算

        // Chunkがいっぱいになった場合
        if (m_max_chunk_entity_max <= m_empty_index) [[unlikely]] {
            addChunk();
            m_empty_index = 0;
        }

        // CDをコンストラクト
        construct(chunk_index, index);
        return std::make_pair(*entity_ptr, chunk_index);
    }

    ArchetypeNumber GetNumber() const {
        return m_archetype_number;
    }

    template<CdConcept... CD>
    PtrTuple<CD...> GetTypes(ChunkIndex chunk_index, uint32 index)
    {
        PtrTuple<CD...> result = { getCD<CD>(chunk_index, index)... };
        return result;
    }

    template<CdOrEntityConcept CdOrEntity>
    std::vector<ArrayView<CdOrEntity>> GetTypeArrays() {
        std::vector<ArrayView<CdOrEntity>> result;
        result.reserve(m_chunks.size());

        for (auto& chunk : m_chunks) {
            result.push_back(chunk->GetArray<CdOrEntity>(getCdIndex<CdOrEntity>()));
        }
        return result;
    }

private:
    /// \ret_val 追加したchunkの最初の要素を示すindexを返す
    void addChunk()
    {
        auto chunk = std::make_shared<Chunk>(
            m_type_infos, 
            m_max_chunk_entity_max,
            m_world_number);
        m_chunks.emplace_back(std::move(chunk));
    }

    void construct(ChunkIndex chunk_index, uint32 index)
    {
        assert(chunk_index < m_chunks.size());
        auto& chunk = m_chunks[chunk_index];

        // MEMO: Entityは初期化しない
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index, 
                index,
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize())
            );
            m_type_infos[cd_index]->Construct(cd);
        }
    }

    Entity* getEntity(ChunkIndex chunk_index, uint32 index)
    {
        assert(chunk_index < m_chunks.size());
        assert(chunk_index < m_max_chunk_entity_max);
        auto& chunk = m_chunks[chunk_index];
        return chunk->GetEntity(index);
    }

    template<CdConcept CD>
    CD* getCD(ChunkIndex chunk_index, uint32 index)
    {
        assert(chunk_index < m_chunks.size());
        assert(chunk_index < m_max_chunk_entity_max);
        auto& chunk = m_chunks[chunk_index];
        return chunk->At<CD>(getCdIndex<CD>(), index);
    }

    template<CdConcept CD>
    CdIndex getCdIndex()const {
        for (CdIndex i = 0; i < m_archetype.size(); ++i) {
            if (m_archetype[i] == TypeIDGenerator<CD>::id()) {
                return i;
            }
        }
        assert(false);
        return kUint8Max;
    }

#if _DEBUG

    static std::string calcArchetypeName(const TypeInfoRefContainer& type_infos)
    {
        std::string archetype_name = "";

        for (const auto& info : type_infos) {
            archetype_name.append(info->GetTypeName());
            archetype_name.append(",");
        }
        return archetype_name;
    }

    std::string m_archetype_name;
#endif // DEBUG

    WorldNumber m_world_number;
    ArchetypeNumber m_archetype_number;
    Archetype m_archetype;

    /// \brief 空きentityへのindex
    uint32 m_empty_index = 0;

    /// \brief 型情報への参照 indexはcd_index
    TypeInfoRefContainer m_type_infos;

    uint32 m_max_chunk_entity_max = 0;
    std::vector<OwnerPtr<Chunk>> m_chunks;

    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> m_brunch;
};