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
/// \brief CDの型ごとに割り振られる数字 0から総数分加算されていく
using CdNumber = uint32;
/// \brief そのArchetypeで何番目のCDか
using CdIndex = uint8;
/// \brief Archetypeの組み合わせごとに割り振られる数字 0から総数分加算されていく
using ArchetypeNumber = uint32;
/// \brief アーキタイプごとのいくつめのchunkに属するかのindex
using ChunkIndex = uint8;
/// \brief chunk内のEntityやCDを示すindex
using LocalIndex = uint32;
using RecordIndex = uint32;
using Generation = uint16;

/// \brief 所有権を持っておきたいポインタ(unique_ptrに置き換え予定)
template<class T>
using OwnerPtr = std::shared_ptr<T>;

/// \brief 保証済みの参照をするだけのポインタ(生ポインタに置き換え予定)
template<class T>
using RefPtr = std::shared_ptr<T>;

/// \brief どの型を持っているかを示す
/// \note numberではなくてID
using Archetype = std::vector<CdID>;

template<class T>
using ToUnderlying = std::underlying_type_t<T>;

template<class... T>
using hana_tuple = boost::hana::tuple<T...>;

template<class... T>
using PtrTuple = std::tuple<T*...>;
