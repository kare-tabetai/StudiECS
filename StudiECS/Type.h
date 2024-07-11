#pragma once
#include "boost_hana.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <type_traits>

class TypeInfo;

using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using TypeDataID = uint32;
using CdID = TypeDataID;
using ArcheTypeID = uint32;

using WorldNumber = uint8;
/// \brief CD�̌^���ƂɊ���U���鐔�� 0���瑍�������Z����Ă���
using CdNumber = uint32;
/// \brief ����Archetype�ŉ��Ԗڂ�CD��
using CdIndex = uint8;
/// \brief Archetype�̑g�ݍ��킹���ƂɊ���U���鐔�� 0���瑍�������Z����Ă���
using ArchetypeNumber = uint32;
/// \brief �A�[�L�^�C�v���Ƃ̂����߂�chunk�ɑ����邩��index
using ChunkIndex = uint8;
/// \brief chunk����Entity��CD������index
using LocalIndex = uint32;
using RecordIndex = uint32;
using Generation = uint16;

/// \brief ���L���������Ă��������|�C���^(unique_ptr�ɒu�������\��)
template<class T>
using OwnerPtr = std::shared_ptr<T>;

/// \brief �ۏ؍ς݂̎Q�Ƃ����邾���̃|�C���^(���|�C���^�ɒu�������\��)
template<class T>
using RefPtr = std::shared_ptr<T>;

/// \brief �ǂ̌^�������Ă��邩������
/// \note number�ł͂Ȃ���ID
using Archetype = std::vector<CdID>;

template<class T>
using ToUnderlying = std::underlying_type_t<T>;

template<class... T>
using hana_tuple = boost::hana::tuple<T...>;

template<class... T>
using PtrTuple = std::tuple<T*...>;
