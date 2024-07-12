#pragma once
#include "ArrayView.h"
#include "OffsetArrayView.h"
#include "ContainerTypes.h"
#include "Concept.h"
#include "Constant.h"
#include "Entity.h"
#include <array>
#include <cstddef>
#include <memory>

/// \brief CDを保存するストレージ
class Chunk {
public:
    Chunk(
        const TypeInfoRefContainer& type_infos_ref,
        uint32 max_entity_count)
        : m_strage()
#if _DEBUG
        , m_max_entity_size(max_entity_count)
#endif
        , m_cd_accessor(createCdArrayAccessor(m_strage, max_entity_count, type_infos_ref))
    {
    }

    template<CdOrEntityConcept CdOrEntity>
    CdOrEntity* At(CdIndex cd_index, LocalIndex local_index)
    {
        assert(cd_index < m_cd_accessor.size());
        assert(local_index < m_max_entity_size);

        return m_cd_accessor[cd_index].At<CdOrEntity>(m_strage.data(), local_index);
    }

    void* At(uint32 cd_index, LocalIndex local_index, uint32 type_size)
    {
        assert(cd_index < m_cd_accessor.size());
        assert(local_index < m_max_entity_size);

        return m_cd_accessor[cd_index].At(m_strage.data(), local_index, type_size);
    }

    Entity* GetEntity(LocalIndex local_index)
    {
        return At<Entity>(0, local_index);
    }

    template<CdOrEntityConcept CD>
    ArrayView<CD> GetArray(uint32 cd_index,uint32 size,uint32 begin_index = 0)
    {
        assert(cd_index < m_cd_accessor.size());
        assert(size < m_max_entity_size);
        assert(begin_index < size);

        auto array_view = m_cd_accessor[cd_index].ToArrayView<CD>(
            m_strage.data(), 
            size,
            begin_index
        );

        assert(Util::IsInRange(array_view.begin(), m_strage.data(), &m_strage.back() + 1));
        assert(Util::IsInRange(array_view.end(), m_strage.data(), &m_strage.back() + 1));


        return array_view;
    }

    /// \brief beginをつぶすように1つずつ要素をずらす 最後の要素が空くはず
    void Shrink(EntityIndex begin, EntityIndex end, const TypeInfoRefContainer& type_infos)
    {
        // TODO: moveかコピーを用いてずらす
    }

    /// \brief 1chunkに含められる最大のentity数を返す
    static uint32 CalcMaxEntityCount(const TypeInfoRefContainer& types_ref)
    {
        uint32 sum_size = 0;
        uint32 sum_align = 0;
        for (const auto& info : types_ref) {
            sum_size += static_cast<uint32>(info->GetTypeSize());
            sum_align += static_cast<uint32>(info->GetAlignSize());
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

#if _DEBUG
    /// \brief chunkに入れられるentityの最大数
    /// \note assert用
    uint32 m_max_entity_size = 0;
#endif // DEBUG

    std::array<std::byte, kChunkSize> m_strage ;
    std::vector<OffsetArrayView> m_cd_accessor = {};
};