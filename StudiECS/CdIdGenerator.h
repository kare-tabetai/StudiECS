#pragma once
#include "Type.h"
#include "Concept.h"

struct CdIdCounter {
    static inline std::atomic<uint32> counter = 0;
};

template<CdOrEntityConcept CD>
struct CdIdGenerator {
    static CdNumber number()
    {
        static const uint32 number = CdIdCounter::counter;
        CdIdCounter::counter++;
        return number;
    }
};
