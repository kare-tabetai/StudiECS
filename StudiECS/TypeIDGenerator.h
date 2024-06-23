#pragma once
#include "Type.h"
#include "MurMur3.h"
#include <functional>
#include <string_view>

struct TypeIDCounter {
    static inline uint32 counter = 0;
};

//https://skypjack.github.io/2020-03-14-ecs-baf-part-8/
template<class Type>
struct TypeIDGenerator {

    static consteval uint32 id()
    {
        return murmur3::to_u32hash(__FUNCSIG__, std::size(__FUNCSIG__));
    }

    static uint32 number() {
        static const uint32 number = TypeIDCounter::counter;
        TypeIDCounter::counter++;
        return number;
    }
};

