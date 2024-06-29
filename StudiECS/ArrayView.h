#pragma once
#include "Type.h"
#include <cassert>

/// \brief �z��r���[�N���X
/// \note std::span�ƈႢ���s���ɃT�C�Y�̎w�肪�ł���
template<class T>
class ArrayView {
public:
    ArrayView(T* _begin,
        uint32 _size)
        : m_begin(_begin)
        , m_size(_size)
    {
    }

    const T& operator[](uint32 index) const
    {
        return const_cast<ArrayView<T>*>(this)[index];
    }

    T& operator[](uint32 index)
    {
        assert(index < m_size);
        return m_begin[index];
    }

    uint32 size() const {
        return m_size;
    }

    T* begin() { return m_begin; }
    T* end() { return begin() + size(); }

private:
    T* m_begin = nullptr;
    uint32 m_size = 0;
};