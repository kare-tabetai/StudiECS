#pragma once
#include "Type.h"
#include "Concept.h"
#include "TypeIDGenerator.h"

struct ArchetypeIdCounter {
    static inline uint32 counter = 0;
};

template<class SanitizedTypeList>
struct ArchetypeIDGenerator {
    static consteval ArcheTypeID id()
    {
        return TypeIDGenerator<ArchetypeIDGenerator<SanitizedTypeList>>::id();
    }

    static uint32 number()
    {
        static const uint32 number = ArchetypeIdCounter::counter;
        ArchetypeIdCounter::counter++;
        return number;
    }
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
