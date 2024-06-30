#pragma once
#include "ArchetypeInfo.h"
#include "ArrayView.h"
#include <array>

void ChunkTest()
{
    constexpr uint32 int_size = sizeof(int);
    constexpr uint32 int_align = alignof(int);

    std::array<int, 18> int_array;
    std::fill(int_array.begin(), int_array.end(), -1);

    ArrayView<int> int_view(int_array.data(), static_cast<uint32>(int_array.size()));
    int_view[1] = 56;
    int_view[3] = 35;
    int_view[8] = 7;
    int_view[3] = 321;

    using Bool = bool;
    constexpr uint32 bool_size = sizeof(Bool);
    constexpr uint32 bool_align = alignof(Bool);

    std::array<Bool, 1024> bool_array;
    std::fill(bool_array.begin(), bool_array.end(), false);

    ArrayView<bool> bool_view(bool_array.data(), static_cast<uint32>(bool_array.size()));
    bool_view[0] = true;
    bool_view[1023] = true;
    bool_view[3] = true;
    bool_view[5] = true;
    bool_view[11] = true;

}