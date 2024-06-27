#pragma once
#include <boost/hana.hpp>
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

/// \brief ���L���������Ă��������|�C���^(unique_ptr�ɒu�������\��)
template<class T>
using OwnerPtr = std::shared_ptr<T>;

/// \brief �ۏ؍ς݂̎Q�Ƃ����邾���̃|�C���^(���|�C���^�ɒu�������\��)
template<class T>
using RefPtr = std::shared_ptr<T>;

using Archetype = std::vector<CdNumber>;

template<class... T>
using hana_tuple = boost::hana::tuple<T...>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;