#pragma once
#include "Type.h"
#include <cassert>

// TODO: std::span‚É’u‚«Š·‚¦
template<class T>
class ArrayView {
public:
    ArrayView(T* _begin,
        size_t _size)
        : m_begin(_begin)
        , m_size(static_cast<uint32>(_size))
    {
    }

    const T& operator[](uint32 index) const
    {
        return const_cast<ArrayView<T>*>(this)[index];
    }

    T& operator[](uint32 index)
    {
        assert(index < m_size);
        T* casted_ptr = static_cast<T*>(m_begin);
        return casted_ptr[index];
    }

private:
    void* m_begin = nullptr;
    uint32 m_size = 0;
};