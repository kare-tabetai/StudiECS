#pragma once
#include "ArrayView.h"
#include <array>

void ChunkTest()
{
    constexpr uint32 int_size = sizeof(int);
    constexpr uint32 int_align = alignof(int);

    std::array<int, 1024> int_array;
    std::fill(int_array.begin(), int_array.end(), -1);
    ArrayView view(int_array.data(), int_array.size());
    *view.At<int>(1) = 56;
    *view.At<int>(3) = 35;
    int* ptr_8 = static_cast<int*>(view.At(8, int_size, int_align));
    *ptr_8 = 7;
    int* ptr_3 = static_cast<int*>(view.At(3, int_size, int_align));
    *ptr_3 = 321;

}