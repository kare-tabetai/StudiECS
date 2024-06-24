#pragma once
#include "Type.h"
#include "TypeIDGenerator.h"

template<class SanitizedTypeList>
struct ArchetypeIDGenerator {
    static consteval ArcheTypeID m_id()
    {
        return TypeIDGenerator<ArchetypeIDGenerator<SanitizedTypeList>>::m_id();
    }
};

template<class Type>
struct CdIdGenerator {
    static constexpr CdID m_id()
    {
        return TypeIDGenerator<CdIdGenerator<Type>>::m_id();
    }

    static CdNumber number()
    {
        return TypeIDGenerator<CdIdGenerator<Type>>::number();
    }
};