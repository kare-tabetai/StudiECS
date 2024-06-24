#pragma once
#include "ArrayView.h"
#include "OffsetArrayView.h"
#include <array>
#include <cstddef>
#include <memory>

/// \brief CD��ۑ�����X�g���[�W
class Chunk {
public:
    Chunk(const TypeInfoRefContainer& type_infos_ref, uint32 max_entity_count)
        : m_chunk()
        , max_entity_count(max_entity_count)
        , cd_accessor(createCdArrayAccessor(type_infos_ref))
    {
    }

    template<class CD>
    CD* At(uint32 cd_index, uint32 entity_index)
    {
        assert(cd_index < cd_accessor.size());
        assert(entity_index < max_entity_count);

        return cd_accessor[cd_index][entity_index];
    }

    template<class CD>
    ArrayView<CD> GetArray(uint32 cd_index)
    {
        assert(cd_index < cd_accessor.size());

        return cd_accessor[cd_index].ToArrayView<CD>(m_chunk.data(), max_entity_count);
    }

    /// \brief 1chunk�Ɋ܂߂���ő��entity����Ԃ�
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

    std::vector<OffsetArrayView> createCdArrayAccessor(const TypeInfoRefContainer& type_infos_ref)
    {
        std::vector<OffsetArrayView> array_views;
        const std::byte* end_ptr = m_chunk.data() + m_chunk.size();
        std::byte* ptr = m_chunk.data();
        for (const auto& info : type_infos_ref) {

            // �^���ŃA���C�������z��擪�ʒu���v�Z
            size_t space = end_ptr - ptr;
            void* v_ptr = ptr;
            ptr = static_cast<std::byte*>(
                std::align(info->GetAlignSize(), info->GetTypeSize(), v_ptr, space));

            // �z��擪�ʒu����̃I�t�Z�b�g�T�C�Y���v�Z
            auto offset_byte = ptr - m_chunk.data();

            array_views.emplace_back(static_cast<uint32>(offset_byte));

            // Entity�̍ő吔*CD�̃T�C�Y�����炷
            ptr += info->GetTypeSize() * max_entity_count;
        }

        return array_views;
    }

    std::array<std::byte, kChunkSize> m_chunk;
    uint32 max_entity_count = 0;
    std::vector<OffsetArrayView> cd_accessor;
};