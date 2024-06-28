#pragma once
#include "ArrayView.h"
#include "OffsetArrayView.h"
#include "ContainerTypes.h"
#include "Concept.h"
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

        return m_cd_accessor[cd_index].ToArrayView<CD>(m_strage.data(), m_max_entity_count);
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
    static constexpr size_t kChunkSize = 64 * 1024; // 64KB

    static std::vector<OffsetArrayView> createCdArrayAccessor(
        std::array<std::byte, kChunkSize>& chunk,
        uint32 max_entity_count,
        const TypeInfoRefContainer& type_infos_ref)
    {
        assert(type_infos_ref.front()->GetID() == TypeIDGenerator<Entity>::id());

        std::vector<OffsetArrayView> array_views;
        const std::byte* end_ptr = chunk.data() + chunk.size();
        std::byte* ptr = chunk.data();
        for (const auto& info : type_infos_ref) {
            if (info->IsEmptyType()) {
                array_views.emplace_back(0);
            }

            // 型情報でアラインした配列先頭位置を計算
            size_t space = end_ptr - ptr;
            void* v_ptr = ptr;
            ptr = static_cast<std::byte*>(
                std::align(info->GetAlignSize(), info->GetTypeSize(), v_ptr, space));

            // 配列先頭位置からのオフセットサイズを計算
            auto offset_byte = ptr - chunk.data();

            array_views.emplace_back(static_cast<uint32>(offset_byte));

            // Entityの最大数*CDのサイズ分ずらす
            ptr += info->GetTypeSize() * max_entity_count;
        }

        return array_views;
    }

    void initializeEntities(
        WorldNumber world_number,
        ArchetypeNumber archetype_number,
        ChunkIndex chunk_index)
    {
        auto entity_array = GetArray<Entity>(0);
        for (uint32 i = 0; i < entity_array.size();++i) {
            EntityIndex entity_index;
            entity_index.world_number = world_number;
            entity_index.archetype_number = archetype_number;
            entity_index.chunk_index = chunk_index;
            entity_index.index = i;
            entity_array[i] = Entity(entity_index);
        }
    }

    std::array<std::byte, kChunkSize> m_strage ;
    uint32 m_max_entity_count = 0;
    std::vector<OffsetArrayView> m_cd_accessor = {};
};