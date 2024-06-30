#pragma once
#include "Entity.h"
#include "Constant.h"
#include <type_traits>

template<class T>
concept MoveOrCopyable = std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

template<class T>
concept CdConcept = MoveOrCopyable<T>
    && DefaultConstructible<T>
    && !std::is_same_v<T, Entity>
    && !std::is_pointer_v<T>
    && (sizeof(T) < kChunkSize);

template<class T>
concept CdOrEntityConcept = CdConcept<T> || std::is_same_v<T, Entity>;

template<typename T>
concept ArithmeticConcept = std::integral<T> || std::floating_point<T>;