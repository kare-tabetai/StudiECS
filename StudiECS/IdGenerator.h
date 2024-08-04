#pragma once
#include "Type.h"
#include "Concept.h"
#include "TypeIDGenerator.h"

struct ArchetypeIdCounter {
    static inline uint32 counter = 0;
};

struct CdIdCounter {
    static inline uint32 counter = 0;
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
