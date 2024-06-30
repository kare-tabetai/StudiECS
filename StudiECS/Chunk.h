#pragma once
#include "ArrayView.h"
#include "OffsetArrayView.h"
#include "ContainerTypes.h"
#include "Concept.h"
#include "Constant.h"
#include <array>
#include <cstddef>
#include <memory>

/// \brief CDを保存するストレージ
class Chunk {
public:
    Chunk(
        const TypeInfoRefContainer& type_infos_ref,
        uint32 max_entity_count, 
        WorldNumber world_number,
        ArchetypeNumber archetype_number,
        ChunkIndex chunk_index)
        : m_strage()
        , m_max_entity_count(max_entity_count)
        , m_cd_accessor(createCdArrayAccessor(m_strage, m_max_entity_count, type_infos_ref ))
    {
        initializeEntities(world_number, archetype_number,chunk_index);
    }

    template<CdOrEntityConcept CdOrEntity>
    CdOrEntity* At(uint32 cd_index, uint32 entity_index)
    {
        assert(cd_index < m_cd_accessor.size());
        assert(entity_index < m_max_entity_count);

        return m_cd_accessor[cd_index].At<CdOrEntity>(m_strage.data(),entity_index);
    }

    void* At(uint32 cd_index, uint32 entity_index, uint32 type_size)
    {
        assert(cd_index < m_cd_accessor.size());
        assert(entity_index < m_max_entity_count);

        return m_cd_accessor[cd_index].At(m_strage.data(), entity_index, type_size);
    }

    Entity* GetEntity(uint32 index) {
        return At<Entity>(0, index);
    }

    template<CdOrEntityConcept CD>
    ArrayView<CD> GetArray(uint32 cd_index)
    {
        assert(cd_index < m_cd_accessor.size());

        auto array_view = m_cd_accessor[cd_index].ToArrayView<CD>(
            m_strage.data(), 
            m_max_entity_count
        );

        assert(Util::IsInRange(array_view.begin(), m_strage.data(), &m_strage.back() + 1));
        assert(Util::IsInRange(array_view.end(), m_strage.data(), &m_strage.back() + 1));


        return array_view;
    }

    /// \brief 1chunkに含められる最大のentity数を返す
    static uint32 CalcMaxEntityCount(const TypeInfoRefContainer& types_ref)
    {
        uint32 sum_size = 0;
        uint32 sum_align = 0;
        for (const auto& info : types_ref) {
            sum_size += info->GetTypeSize();
            sum_align += info->GetAlignSize();
        }

        return (kChunkSize - sum_align) / sum_size;
    }

private:
    static std::vector<OffsetArrayView> createCdArrayAccessor(
        std::array<std::byte, kChunkSize>& strage,
        uint32 max_entity_count,
        const TypeInfoRefContainer& type_infos_ref)
    {
        assert(type_infos_ref.front()->GetID() == TypeIDGenerator<Entity>::id());

        std::vector<OffsetArrayView> array_views;
        const std::byte* end_ptr = strage.data() + strage.size();

        std::byte* ptr = strage.data();
        for (const auto& info : type_infos_ref) {
            if (info->IsEmptyType()) {
                array_views.emplace_back(0);
            }

            size_t space = end_ptr - ptr;//残りのスペース
            void* tmp = ptr;
            // 型情報でアラインした配列先頭位置を計算
            std::align(info->GetAlignSize(), info->GetTypeSize(), tmp, space);
            ptr = static_cast<std::byte*>(tmp);

            // 配列先頭位置からのオフセットサイズを計算
            auto offset_byte = ptr - strage.data();
            assert(strage.data() + offset_byte <= end_ptr);

            array_views.emplace_back(static_cast<uint32>(offset_byte));

            // Entityの最大数*CDのサイズ分ずらす
            auto array_byte_size = info->GetTypeSize() * max_entity_count;
            ptr += array_byte_size;
            assert(Util::IsInRange(ptr, strage.data(), &strage.back() + 1));
        }

        return array_views;
    }

    void initializeEntities(
        WorldNumber world_number,
        ArchetypeNumber archetype_number,
        ChunkIndex chunk_index)
    {
        auto entity_array = GetArray<Entity>(0);
        for (uint32 i = 0; i < entity_array.size()-1;++i) {
            assert(Util::IsInRange(&entity_array[i], m_strage.data(), &m_strage.back() + 1));

            EntityIndex entity_index;
            entity_index.world_number = world_number;
            entity_index.archetype_number = archetype_number;
            entity_index.chunk_index = chunk_index;
            // MEMO: 未使用のEntityは未使用リストとして扱うため
            // 次の未使用の要素(次のindex)をさすようにしている
            entity_index.index = i+1;
            entity_array[i] = Entity(entity_index,Entity::Flag::IsInChunk & Entity::Flag::Invalid);
        }
    }

    std::array<std::byte, kChunkSize> m_strage ;
    uint32 m_max_entity_count = 0;
    std::vector<OffsetArrayView> m_cd_accessor = {};
};