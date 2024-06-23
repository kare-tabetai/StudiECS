#pragma once
#include "TypeInfo.h"
#include <vector>
#include <unordered_map>
#include <memory>

using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using CdID = uint32;
using CdNumber = uint32;
using ChunkContainerIndex = uint32;
using ArcheTypeID = uint32;
using Archetype = std::vector<CdID>;
using TypeInfoContainer = std::unordered_map<CdID, std::shared_ptr<TypeInfo>>;
using TypeInfoRefContainer = std::vector<std::weak_ptr<TypeInfo>>;