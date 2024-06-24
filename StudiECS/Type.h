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

/// \brief 所有権を持っておきたいポインタ(unique_ptrに置き換え予定)
template<class T>
using OwnerPtr = std::shared_ptr<T>;

/// \brief 保証済みの参照をするだけのポインタ(生ポインタに置き換え予定)
template<class T>
using RefPtr = std::shared_ptr<T>;

using Archetype = std::vector<TypeDataID>;
using TypeInfoContainer = std::unordered_map<TypeDataID, OwnerPtr<TypeInfo>>;
using TypeInfoRefContainer = std::vector<RefPtr<TypeInfo>>;

