#pragma once
#include "Type.h"
#include <cassert>

class ArrayView {
public:
    constexpr ArrayView(void* _begin,
        size_t _size)
        : m_begin(_begin)
        , m_size(_size)
    {
    }

    template<class T>
    constexpr T* At(size_t index)
    {
        assert(index < m_size);
        T* casted_ptr = static_cast<T*>(m_begin);
        return &casted_ptr[index];
    }

    void* At(size_t index, uint32 type_size, uint32 type_align)
    {
        assert(index < m_size);
        void* ptr = m_begin;
        std::align(type_align, type_size, ptr, index);
        return ptr;
    }

private:
    void* m_begin = nullptr;
    size_t m_size = 0;
};