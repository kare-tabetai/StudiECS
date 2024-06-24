#pragma once
#include <vector>
#include <unordered_map>
#include <memory>

class TypeInfo;

using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using TypeDataID = uint32;
using CdID = TypeDataID;
using CdNumber = uint32;
using ChunkContainerIndex = uint32;
using ArcheTypeID = uint32;

/// \brief ���L���������Ă��������|�C���^(unique_ptr�ɒu�������\��)
template<class T>
using OwnerPtr = std::shared_ptr<T>;

/// \brief �ۏ؍ς݂̎Q�Ƃ����邾���̃|�C���^(���|�C���^�ɒu�������\��)
template<class T>
using RefPtr = std::shared_ptr<T>;

using Archetype = std::vector<TypeDataID>;
using TypeInfoContainer = std::unordered_map<TypeDataID, OwnerPtr<TypeInfo>>;
using TypeInfoRefContainer = std::vector<RefPtr<TypeInfo>>;

