#pragma once
#include "Type.h"
#include <cassert>

/// \brief 配列ビュークラス
/// \note std::spanと違い実行時にサイズの指定ができる
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
        T* casted_ptr = static_cast<T*>(m_begin);
        return casted_ptr[index];
    }

    uint32 size() const {
        return m_size;
    }

    T* begin() { return static_cast<T*>(m_begin); }
    T* end() { return begin() + size(); }

private:
    void* m_begin = nullptr;
    uint32 m_size = 0;
};