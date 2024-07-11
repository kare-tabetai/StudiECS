#pragma once
#include "ArchetypeBrunch.h"
#include "Chunk.h"
#include "OffsetArrayView.h"
#include "Type.h"
#include "Entity.h"
#include "ContainerTypes.h"
#include <unordered_map>
#include "EntityIndex.h"

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
        , m_max_entity_size(Chunk::CalcMaxEntityCount(type_infos))
        , m_chunks()
        , m_brunch()
    {
        addChunk();
    }

    /// \ret_val Entityの参照とそのindexを返す
    std::tuple<Entity, EntityIndex> CreateEntity(RecordIndex record_index, Generation generation)
    {
        // 最後のchunk,空きindex
        EntityIndex entity_index = EntityIndex(static_cast<ChunkIndex>(m_chunks.size() - 1), m_empty_index);
        auto* entity_ptr = getEntity(entity_index);
        assert(entity_ptr->IsInvalid());

        entity_ptr->ReSet(record_index, generation, m_world_number); // 使用中にする

        m_empty_index++; // 空きentityを指すように加算

        // Chunkがいっぱいになった場合
        if (m_max_entity_size <= m_empty_index) [[unlikely]] {
            addChunk();
            m_empty_index = 0;
        }

        // CDをコンストラクト
        construct(entity_index);

        return std::make_tuple(*entity_ptr, entity_index);
    }

    /// \brief Entityを削除する
    /// \ret_val 削除されたEntity以降のindexのEntity
    std::vector<ArrayView<Entity>> DestroyEntity(const EntityIndex& entity_index)
    {
        destruct(entity_index);

        if (entity_index.IsLast(m_chunks.size(), m_max_entity_size)) {
            // 最後の要素の場合は空で返す
            return std::vector<ArrayView<Entity>>();
        }

        auto next_index = entity_index.GetIncremented(m_chunks.size(), m_max_entity_size);
        return GetTypeArrays<Entity>(next_index);
    }

    ArchetypeNumber GetNumber() const {
        return m_archetype_number;
    }

    template<CdConcept... CD>
    PtrTuple<CD...> GetTypes(const EntityIndex& entity_index)
    {
        PtrTuple<CD...> result = { getCD<CD>(entity_index)... };
        return result;
    }

    template<CdOrEntityConcept CdOrEntity>
    std::vector<ArrayView<CdOrEntity>> GetTypeArrays(const EntityIndex& begin_entity_index = kBeginEntityIndex)
    {
        if (m_chunks.empty()) {
            return std::vector<ArrayView<CdOrEntity>>();
        }

        assert(begin_entity_index.InRange(m_chunks.size(), m_max_entity_size, m_empty_index));
        
        std::vector<ArrayView<CdOrEntity>> result;

        if (begin_entity_index.IsLastChunk(m_chunks.size())) {
            result.push_back(
                m_chunks.back()->GetArray<CdOrEntity>(
                    getCdIndex<CdOrEntity>(), 
                    m_empty_index, 
                    begin_entity_index.GetLocalIndex())
            );
            return result;
        }

        result.reserve(m_chunks.size());
        
        for (uint32 i = begin_entity_index.GetChunkIndex(); i < m_chunks.size() - 1; ++i) {
            result.push_back(
                m_chunks[i]->GetArray<CdOrEntity>(
                    getCdIndex<CdOrEntity>(), 
                    m_max_entity_size,
                    begin_entity_index.GetLocalIndex()
                )
            );
        }

        assert(m_empty_index != 0);
        result.push_back(m_chunks.back()->GetArray<CdOrEntity>(getCdIndex<CdOrEntity>(), m_empty_index ));

        return result;
    }

private:
    /// \ret_val 追加したchunkの最初の要素を示すindexを返す
    void addChunk()
    {
        auto chunk = std::make_shared<Chunk>(
            m_type_infos, 
            m_max_entity_size
            );
        m_chunks.emplace_back(std::move(chunk));
    }

    void construct(EntityIndex entity_index)
    {
        assert(entity_index.InRange(m_chunks.size(), m_max_entity_size, m_empty_index));
        auto& chunk = m_chunks[entity_index.GetChunkIndex()];

        // MEMO: Entityは呼ばない
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index, 
                entity_index.GetLocalIndex(),
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize())
            );
            m_type_infos[cd_index]->Construct(cd);
        }
    }

    void destruct(const EntityIndex& entity_index)
    {
        assert(entity_index.InRange(m_chunks.size(),m_max_entity_size,m_empty_index));
        auto& chunk = m_chunks[entity_index.GetChunkIndex()];

        // MEMO: Entityは呼ばないため1から開始
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index,
                entity_index.GetLocalIndex(),
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize()));
            m_type_infos[cd_index]->Destruct(cd);
        }
    }

    Entity* getEntity(const EntityIndex& entity_index)
    {
        assert(entity_index.InRange(m_chunks.size(), m_max_entity_size, m_empty_index+1));
        auto& chunk = m_chunks[entity_index.GetChunkIndex()];
        return chunk->GetEntity(entity_index.GetLocalIndex());
    }

    template<CdConcept CD>
    CD* getCD(const EntityIndex& entity_index)
    {
        assert(entity_index.InRange(m_chunks.size(), m_max_entity_size, m_empty_index));
        auto& chunk = m_chunks[entity_index.GetChunkIndex()];
        return chunk->At<CD>(getCdIndex<CD>(), entity_index.GetLocalIndex());
    }

    template<CdOrEntityConcept CdOrEntity>
    CdIndex getCdIndex()const {
        for (CdIndex i = 0; i < m_archetype.size(); ++i) {
            if (m_archetype[i] == TypeIDGenerator<CdOrEntity>::id()) {
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

    /// \brief chunkごとに入れられるentityの数
    uint32 m_max_entity_size = 0;
    std::vector<OwnerPtr<Chunk>> m_chunks;

    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, ArchetypeBrunch> m_brunch;
};