#pragma once
#include "Type.h"
#include "TypeIDGenerator.h"

template<class SanitizedTypeList>
struct ArchetypeIDGenerator {
    static consteval ArcheTypeID id()
    {
        return TypeIDGenerator<ArchetypeIDGenerator<SanitizedTypeList>>::id();
    }
};

template<class Type>
struct CdIdGenerator {
    static constexpr CdID id()
    {
        return TypeIDGenerator<CdIdGenerator<Type>>::id();
    }

    static CdNumber number()
    {
        return TypeIDGenerator<CdIdGenerator<Type>>::number();
    }
};