#pragma once
#include "Type.h"
#include "ArrayView.h"

class OffsetArrayView {
public:
    OffsetArrayView(uint32 _offset_byte)
        : offset_byte(_offset_byte)
    {
    }

    template<class T>
    T* At(void* head_ptr, size_t index)
    {
        std::byte* ptr = static_cast<std::byte*>(head_ptr);
        ptr += offset_byte;
        T* casted_ptr = static_cast<T*>(ptr);
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
    ArrayView<T> ToArrayView(void* head_ptr, uint32 size) {
        std::byte* byte_ptr = head_ptr;
        byte_ptr += offset_byte;
        auto* array_begin_ptr = static_cast<T*>(byte_ptr);

        return ArrayView<T>(array_begin_ptr, size);
    }

private:
    uint32 offset_byte = 0;
};