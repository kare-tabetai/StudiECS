#pragma once
#include "SparseSet.h"
#include <iostream>

void SparseSetTest() {
    SparseSet<uint32> int_set;

    for (size_t i = 0; i < 100; i++) {
        int_set[i] = i;
    }
    for (size_t i = 0; i < 100; i++) {
        std::cout << i << ":" << int_set[i] << "\n";
    }
    for (size_t i = 0; i < 100; i = i + 2) {
        int_set.Erase(i);
    }
    for (size_t i = 0; i < 100; i++) {
        std::cout << i << ":" << int_set[i] << "\n";
    }
}