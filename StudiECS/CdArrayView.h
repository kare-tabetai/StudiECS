#pragma once
#include "Concept.h"
#include "ArrayView.h"
#include <vector>

// TODO: �N���X�����ĒP��̃C�e���[�^�[�Ń��[�v�ł���悤�ɂ���
template<CdOrEntityConcept CdOrEntity>
using CdArrayView = std::vector<ArrayView<CdOrEntity>>;

template<CdOrEntityConcept... CdOrEntity>
using CdArrayViews = std::tuple<CdArrayView<CdOrEntity>...>;