#pragma once
#include "Concept.h"
#include "ArrayView.h"
#include <vector>

// TODO: クラス化して単一のイテレーターでループできるようにする
template<CdOrEntityConcept CdOrEntity>
using CdArrayView = std::vector<ArrayView<CdOrEntity>>;

template<CdOrEntityConcept... CdOrEntity>
using CdArrayViews = std::tuple<CdArrayView<CdOrEntity>...>;