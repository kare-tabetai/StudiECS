#pragma once
#include "Entity.h"
#include "Constant.h"
#include <type_traits>

template<class T>
concept MoveOrCopyable = std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

/// \brief CDに利用できる型の制限
template<class T>
concept CdConcept = MoveOrCopyable<T>
    && DefaultConstructible<T> //現状はDefaultConstruct後に値を変更するインターフェースを想定しているため
    && !std::is_same_v<T, Entity>// Entityはこちらで追加するため
    && !std::is_pointer_v<T>//実装が面倒になりそうなため、参照はDefaultConstructibleではじいている
    && (sizeof(T) <= kUint32Max);//内部でkUint32Max以下を前提に組んでいるため

template<class T>
concept CdOrEntityConcept = CdConcept<T> || std::is_same_v<T, Entity>;

template<typename T>
concept ArithmeticConcept = std::integral<T> || std::floating_point<T>;