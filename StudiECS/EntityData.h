#pragma once
#include "ArchetypeInfo.h"
#include "Type.h"
#include "EntityIndex.h"

struct EntityData {
    RefPtr<ArchetypeInfo> archetype_ref;
    EntityIndex entity_index;
};