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

/// \brief CD��ۑ�����X�g���[�W
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
    ArrayView<CD> GetArray(CdIndex cd_index, uint32 size, uint32 begin_index = 0)
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

    /// \brief begin���Ԃ��悤��1����Asign�ł��炷 end - 1�̗v�f���f�X�g���N�g�����
    void Shrink(LocalIndex begin, LocalIndex end, const TypeInfoRefContainer& type_infos)
    {
        assert(begin < end);
        for (CdIndex cd_index = 0; cd_index < type_infos.size(); cd_index++) {
            const auto& type_info = type_infos[cd_index];
            if (type_info->IsEmptyType()) {
                continue;
            } else if (type_info->CanTrivialCopy()) {
                //TODO: std::memmove�ňꊇ�œ�����
            }

            for (LocalIndex local_index = begin; local_index < end-1; local_index++) {
                void* dest = At(cd_index, local_index, type_info->GetTypeSize());
                void* source = At(cd_index, local_index+1, type_info->GetTypeSize());

                if (type_info->MoveAsign(source, dest)) {
                    // move���ł����玟�̗v�f��
                    continue;
                }
                if (type_info->CopyAsign(source, dest)) {
                    // copy���ł����玟�̗v�f��
                    continue;
                }
                // CD�ɂ�copyable��������moveable��concept���w�肵�Ă��邽�ߗ���͂����Ȃ�
                assert(false);
            }
            void* last = At(cd_index, end-1, type_info->GetTypeSize());
            type_info->Destruct(last);
        }
    }

    /// \brief 1chunk�Ɋ܂߂���ő��entity����Ԃ�
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

            size_t space = end_ptr - ptr;//�c��̃X�y�[�X
            void* tmp = ptr;
            // �^���ŃA���C�������z��擪�ʒu���v�Z
            std::align(info->GetAlignSize(), info->GetTypeSize(), tmp, space);
            ptr = static_cast<std::byte*>(tmp);

            // �z��擪�ʒu����̃I�t�Z�b�g�T�C�Y���v�Z
            auto offset_byte = ptr - strage.data();
            assert(strage.data() + offset_byte <= end_ptr);

            array_views.emplace_back(static_cast<uint32>(offset_byte));

            // Entity�̍ő吔*CD�̃T�C�Y�����炷
            auto array_byte_size = info->GetTypeSize() * max_entity_count;
            ptr += array_byte_size;
            assert(Util::IsInRange(ptr, strage.data(), &strage.back() + 1));
        }

        return array_views;
    }

#if _DEBUG
    /// \brief chunk�ɓ������entity�̍ő吔
    /// \note assert�p
    uint32 m_max_entity_size = 0;
#endif // DEBUG

    std::array<std::byte, kChunkSize> m_strage ;
    std::vector<OffsetArrayView> m_cd_accessor = {};
};