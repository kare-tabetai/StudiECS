#pragma once
#include "Chunk.h"
#include "Type.h"
#include "Entity.h"
#include "ContainerTypes.h"
#include "CdArrayView.h"
#include "TypeUtil.h"
#include <unordered_map>

class ArchetypeInfo {
public:
    ArchetypeInfo(
        const Archetype& archetype,
        const TypeInfoRefContainer& type_infos,
        WorldNumber world_number)
        : 
#if _DEBUG
        m_archetype_name(calcArchetypeName(type_infos))
        ,
#endif // DEBUG
        m_world_number(world_number)
        , m_archetype(archetype)
        , m_type_infos(type_infos)
        , m_max_entity_size(Chunk::CalcMaxEntityCount(type_infos))
        , m_chunks()
    {
        assert(m_max_entity_size != 0);
        addChunk();
    }

    /// \retval Entityの参照とそのindexを返す
    std::pair<Entity, EntityIndex> CreateEntity(RecordIndex record_index, Generation generation)
    {
        assert(!m_chunks.empty());

        EntityIndex index = static_cast<EntityIndex>(m_chunks.size() - 1) * m_max_entity_size + m_last_chunk_entity_size;
        Entity* entity = m_chunks.back()->GetEntity(m_last_chunk_entity_size);

        entity->ReSet(record_index, generation, m_world_number); // 使用中にする

        // Chunkがいっぱいになった場合
        if (m_max_entity_size <= m_last_chunk_entity_size + 1) [[unlikely]] {
            addChunk();
            m_last_chunk_entity_size = 0;
        } else {
            m_last_chunk_entity_size++; // 空きentityを指すように加算
        }

        // CDをコンストラクト
        construct(index);

        return std::make_pair(*entity, index);
    }

    /// \brief Entityを削除する
    /// \retval 削除されたEntity以降のindexのEntity
    CdArrayView<Entity> DestroyEntity(EntityIndex index)
    {
        assert(!m_chunks.empty());
        shrink(index);
        return GetTypeArrays<Entity>(index);
    }

    template<CdOrEntityConcept... CdOrEntity>
    PtrTuple<CdOrEntity...> GetTypes(EntityIndex index)
    {
        PtrTuple<CdOrEntity...> result = { GetCD<CdOrEntity>(index)... };
        return result;
    }

    template<CdOrEntityConcept... CdOrEntity>
    PtrTuple<const CdOrEntity...> GetTypes(EntityIndex index) const
    {
        PtrTuple<const CdOrEntity...> result = { GetCD<CdOrEntity>(index)... };
        return result;
    }

    std::vector<void*> GetTypesRaw(EntityIndex index)
    {
        std::vector<void*> ret;
        for (CdIndex cd_index = 0; cd_index < m_archetype.size(); ++cd_index) {
            void* raw_cd_ptr = GetCDRaw(cd_index, index);
            ret.push_back(raw_cd_ptr);
        }
        return ret;
    }

    template<CdOrEntityConcept CdOrEntity>
    CdArrayView<CdOrEntity> GetTypeArrays(EntityIndex begin_index = 0)
    {
        if (isEmpty()) {
            return std::vector<ArrayView<CdOrEntity>>();
        }

        assert(m_last_chunk_entity_size != 0);

        uint32 begin_chunk_index = begin_index / m_max_entity_size;
        LocalIndex begin_local_index = begin_index % m_max_entity_size;

        std::vector<ArrayView<CdOrEntity>> result;
        result.reserve(m_chunks.size() - begin_chunk_index);
        
        if (m_chunks.size() == 1) {
            result.push_back(
                m_chunks[begin_chunk_index]->GetArray<CdOrEntity>(
                    getCdIndexByConcept<CdOrEntity>(),
                    m_last_chunk_entity_size,
                    begin_local_index));
        }
        else
        {
            result.push_back(
                m_chunks[begin_chunk_index]->GetArray<CdOrEntity>(
                    getCdIndexByConcept<CdOrEntity>(),
                    m_max_entity_size,
                    begin_local_index));

            // 最後のチャンク以外はchunk内のCdOrEntityの要素をすべて取得する
            for (uint32 i = begin_chunk_index + 1; i < m_chunks.size() - 1; ++i) {
                result.push_back(
                    m_chunks[i]->GetArray<CdOrEntity>(
                        getCdIndexByConcept<CdOrEntity>(),
                        m_max_entity_size));
            }

            // 最後のチャンクはchunk内のCdOrEntityの要素をm_empty_indexのサイズ取得する
            result.push_back(m_chunks.back()->GetArray<CdOrEntity>(getCdIndexByConcept<CdOrEntity>(), m_last_chunk_entity_size));
        }
        
        return result;
    }

    template<CdOrEntityConcept... CdOrEntity>
    CdArrayViews<CdOrEntity...> GetTypesArrays(EntityIndex begin_index = 0)
    {
        return std::make_tuple(GetTypeArrays<CdOrEntity>()...);
    }

    template<CdOrEntityConcept CdOrEntity>
    CdOrEntity* GetCD(EntityIndex index)
    {
        auto& chunk = m_chunks[index / m_max_entity_size];
        return chunk->At<CdOrEntity>(getCdIndexByConcept<CdOrEntity>(), index % m_max_entity_size);
    }

    template<CdOrEntityConcept CdOrEntity>
    const CdOrEntity* GetCD(EntityIndex index) const
    {
        return const_cast<ArchetypeInfo>(this)->GetCD<CdOrEntity>(index);
    }



    template<CdOrEntityConcept FirstCdOrEntity, CdOrEntityConcept... CdOrEntities>
    bool HasAll() const
    {
        bool has_first = has<FirstCdOrEntity>();

        if constexpr (sizeof...(CdOrEntities) == 0) {
            return has_first;
        } else {
            if (!has_first) {
                return false;
            } else {
                return HasAll<CdOrEntities...>();
            }
        }
    }

    void* GetCDRaw(CdIndex cd_index, EntityIndex index) {
        auto& chunk = m_chunks[index / m_max_entity_size];
        auto& type_ref = m_type_infos[cd_index];
        return chunk->At(cd_index, index % m_max_entity_size, static_cast<uint32>( type_ref->GetTypeSize()));
    }

    RefPtr<ArchetypeInfo> TryGetAddCdArchetypeInfo(CdID cd_id) {
        auto itr = m_add_cd_brunch.find(cd_id);
        if (itr != m_add_cd_brunch.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    RefPtr<ArchetypeInfo> TryGetRemoveCdArchetypeInfo(CdID cd_id)
    {
        auto itr = m_remove_cd_brunch.find(cd_id);
        if (itr != m_remove_cd_brunch.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    template<CdConcept CD>
    void RegisterAddCdArchetypeInfo( const RefPtr<ArchetypeInfo>& archetype_info)
    {
        constexpr CdID kCdTypeID = TypeIDGenerator<CD>::id();
        assert(m_add_cd_brunch.count(kCdTypeID) == 0);

        m_add_cd_brunch.try_emplace(kCdTypeID, archetype_info);
    }

    template<CdConcept CD>
    void RegisterRemoveCdArchetypeInfo(const RefPtr<ArchetypeInfo>& archetype_info)
    {
        constexpr CdID kCdTypeID = TypeIDGenerator<CD>::id();
        assert(m_remove_cd_brunch.count(kCdTypeID) == 0);

        m_remove_cd_brunch.try_emplace(kCdTypeID, archetype_info);
    }

    bool IsSameArchetype(const Archetype& archetype) const
    {
        return m_archetype == archetype;
    }

    Archetype GetArcehtype() const {
        return m_archetype;
    }

    TypeInfoRefContainer GetTypeInfoRefContainer() const {
        return m_type_infos;
    }

    std::optional<CdIndex> GetCdIndexByTypeInfo(const RefPtr<TypeInfo>& serch_info)
    {
        auto itr = std::find(m_type_infos.begin(), m_type_infos.end(), serch_info);
        if (itr != m_type_infos.end()) {
            return static_cast<CdIndex>(std::distance(m_type_infos.begin(), itr));
        } else {
            return std::nullopt;
        }
    }

private:
    /// \retval 追加したchunkの最初の要素を示すindexを返す
    void addChunk()
    {
        auto chunk = std::make_unique<Chunk>(
            m_type_infos, 
            m_max_entity_size
            );
        m_chunks.emplace_back(std::move(chunk));
    }

    void construct(EntityIndex index)
    {
        auto& chunk = m_chunks[index / m_max_entity_size];

        // MEMO: Entityは呼ばない
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index, 
                index % m_max_entity_size,
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize())
            );
            m_type_infos[cd_index]->Construct(cd);
        }
    }

    void destruct(EntityIndex index)
    {
        auto& chunk = m_chunks[index / m_max_entity_size];

        // MEMO: Entityは呼ばないため1から開始
        for (uint32 cd_index = 1; cd_index < m_type_infos.size(); cd_index++) {
            void* cd = chunk->At(
                cd_index,
                index % m_max_entity_size,
                static_cast<uint32>(m_type_infos[cd_index]->GetTypeSize()));
            m_type_infos[cd_index]->Destruct(cd);
        }
    }

    /// \brief 指定したindexをつぶすように縮める
    /// \note TODO:高速化のために新しいChunkを作ってそこにずらしてmemcpyを行う
    void shrink(EntityIndex index)
    {
        assert(m_last_chunk_entity_size != 0);
        assert(!m_chunks.empty());

        uint32 erase_chunk_index = index / m_max_entity_size;
        LocalIndex erase_local_index = index % m_max_entity_size;
        auto& chunk = m_chunks[erase_chunk_index];

        if (m_chunks.size() == 1) {
            chunk->Shrink(erase_local_index, m_last_chunk_entity_size, m_type_infos);
            destruct(m_last_chunk_entity_size - 1);
        } else {
            chunk->Shrink(erase_local_index, m_max_entity_size, m_type_infos);
            for (uint32 i = erase_chunk_index + 1; i < m_chunks.size() - 1; i++) {
                auto& prev_chunk = m_chunks[i - 1];
                auto& cur_chunk = m_chunks[i];
                moveOrCopyChunkFlont2ChunkBack(
                    *prev_chunk,
                    *cur_chunk,
                    m_type_infos,
                    m_max_entity_size);

                cur_chunk->Shrink(0, m_max_entity_size, m_type_infos);
            }

            auto& prev_chunk = m_chunks[m_chunks.size() - 2];
            auto& last_chunk = m_chunks.back();
            moveOrCopyChunkFlont2ChunkBack(
                *prev_chunk,
                *last_chunk,
                m_type_infos,
                m_max_entity_size);
            m_chunks.back()->Shrink(0, m_last_chunk_entity_size, m_type_infos);

            EntityIndex last_entity_index = static_cast<sint32>(m_chunks.size() - 1) * m_last_chunk_entity_size + m_last_chunk_entity_size - 1;
            destruct(last_entity_index);
        }

        // Chunkが空になった場合
        if (m_last_chunk_entity_size == 1) [[unlikely]] {
            assert(m_chunks.size() != 0);
            if (1 < m_chunks.size()) {
                m_chunks.pop_back();
                m_last_chunk_entity_size = m_max_entity_size - 1;
            } else if (m_chunks.size() == 1) {
                m_last_chunk_entity_size = 0;
            }

        } else {
            m_last_chunk_entity_size--; // 空きentityを指すように減算
        }
    }

    static void moveOrCopyChunkFlont2ChunkBack(
        Chunk& dest_chunk, 
        Chunk& source_chunk, 
        const TypeInfoRefContainer& type_infos,
        uint32 max_entity_size)
    {
        for (CdIndex cd_index = 0; cd_index < type_infos.size(); cd_index++)
        {
            const auto& type_info = type_infos[cd_index];
            if (type_info->IsEmptyType()) {
                continue;
            } else if (type_info->CanTrivialCopy()) {
                // TODO: std::memmoveで一括で動かす
            }

            uint32 type_size = static_cast<uint32>(type_info->GetTypeSize());
            void* dest = dest_chunk.At(cd_index, max_entity_size - 1, type_size);
            void* source = source_chunk.At(cd_index, max_entity_size - 1 + 1, type_size);
            TypeInfoHelper::MoveOrCopy(*type_info, source, dest);
        }
    }

    Entity* getEntity(EntityIndex index)
    {
        auto& chunk = m_chunks[index / m_max_entity_size];
        return chunk->GetEntity(index % m_max_entity_size);
    }

    template<CdOrEntityConcept CdOrEntity>
    CdIndex getCdIndexByConcept()const {
        auto itr = std::find(
            m_archetype.begin(), 
            m_archetype.end(), 
            TypeIDGenerator<CdOrEntity>::id());
        if (itr != m_archetype.end()) [[likely]] {
            return static_cast<CdIndex>( std::distance(m_archetype.begin(), itr));
        } else {
            assert(false);
            return kUint8Max;
        }
    }
    
    bool isEmpty() const
    {
        return m_chunks.size() == 1 && m_last_chunk_entity_size == 0;
    }

    template<CdOrEntityConcept CdOrEntity>
    bool has() const
    {
        constexpr TypeDataID kTypeDataID = TypeIDGenerator<CdOrEntity>::id();
        auto itr = std::find_if(m_type_infos.begin(), m_type_infos.end(),
            [kTypeDataID](const RefPtr<TypeInfo>& info) {
                return info->GetID() == kTypeDataID;
            });
        return itr != m_type_infos.end();
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
    Archetype m_archetype; //TODO:m_type_infosで事足りるのではないかを検討

    /// \brief 型情報への参照 indexはcd_index
    TypeInfoRefContainer m_type_infos;

    /// \brief 最後のchunkに含まれているenittyの数 空きentityへのindex
    uint32 m_last_chunk_entity_size = 0;

    /// \brief chunkごとに入れられるentityの数
    uint32 m_max_entity_size = 0;

    std::vector<OwnerPtr<Chunk>> m_chunks;

    ///  \brief CD追加時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, const RefPtr<ArchetypeInfo>> m_add_cd_brunch;
    ///  \brief CD削除時に移動するArchetypeへの参照キャッシュ
    std::unordered_map<CdID, const RefPtr<ArchetypeInfo>> m_remove_cd_brunch;
};