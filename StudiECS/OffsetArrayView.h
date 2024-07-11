#pragma once
#include "Type.h"
#include "ArrayView.h"

class OffsetArrayView {
public:
    constexpr OffsetArrayView(uint32 _offset_byte)
        : offset_byte(_offset_byte)
    {
    }

    template<class T>
    T* At(void* head_ptr, size_t index)
    {
        std::byte* ptr = static_cast<std::byte*>(head_ptr);
        ptr += offset_byte;
        T* casted_ptr = reinterpret_cast<T*>(ptr);
        return &casted_ptr[index];
    }

    void* At(void* head_ptr, size_t index, uint32 type_size)
    {
        std::byte* ptr = static_cast<std::byte*>(head_ptr);
        ptr += offset_byte;
        ptr += type_size * index;
        return ptr;
    }

    template<class T>
    ArrayView<T> ToArrayView(void* head_ptr, uint32 size,uint32 begin_index = 0) {
        std::byte* byte_ptr = static_cast<std::byte*>(head_ptr);
        byte_ptr += offset_byte;
        T* array_begin_ptr = reinterpret_cast<T*>(byte_ptr);
        array_begin_ptr += begin_index;

        return ArrayView<T>(array_begin_ptr, size - begin_index);
    }

    constexpr uint32 GetOffsetByte() const { return offset_byte; }

private:
    uint32 offset_byte = 0;
};