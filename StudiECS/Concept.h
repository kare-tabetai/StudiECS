#pragma once
#include "Entity.h"
#include "Constant.h"
#include <type_traits>

template<class T>
concept MoveOrCopyable = std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

/// \brief CD�ɗ��p�ł���^�̐���
template<class T>
concept CdConcept = MoveOrCopyable<T>
    && DefaultConstructible<T> //�����DefaultConstruct��ɒl��ύX����C���^�[�t�F�[�X��z�肵�Ă��邽��
    && !std::is_same_v<T, Entity>// Entity�͂�����Œǉ����邽��
    && !std::is_pointer_v<T>//�������ʓ|�ɂȂ肻���Ȃ��߁A�Q�Ƃ�DefaultConstructible�ł͂����Ă���
    && (sizeof(T) <= kUint32Max);//������kUint32Max�ȉ���O��ɑg��ł��邽��

template<class T>
concept CdOrEntityConcept = CdConcept<T> || std::is_same_v<T, Entity>;

template<typename T>
concept ArithmeticConcept = std::integral<T> || std::floating_point<T>;