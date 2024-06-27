#pragma once
#include "EnumUtil.h"
#include "EntityIndex.h"
#include "Type.h"
#include <bitset>
#include <boost/hana/hash.hpp>

class Entity {
public:
    friend struct std::hash<Entity>;

    enum class Flag : uint8 {
        None = 0,
        Invalid = 1 << 0,
    };

    static constexpr Entity Invalid()
    {
        Entity entity;
        entity.value.index_generation.flags = Flag::Invalid;
        return entity;
    }

    EntityIndex Index() const { return value.index_generation.entity_index; }

    /// \brief –³Œø‚Èentity‚©‚Ç‚¤‚©
    bool IsInvalid() const
    {
        return ToBool(value.index_generation.flags | Flag::Invalid);
    }

    Entity& SetFlag(Flag flag)
    {
        value.index_generation.flags = flag;
        return *this;
    }
    Entity& AddFlag(Flag flag)
    {
        value.index_generation.flags |= flag;
        return *this;
    }

    bool operator==(const Entity& other) const
    {
        return value.index_generation.entity_index == other.value.index_generation.entity_index 
            && value.index_generation.world_number == other.value.index_generation.world_number
            && value.index_generation.generation == other.value.index_generation.generation;
    }

private:
    struct IndexGeneration {
        EntityIndex entity_index ;
        uint8 world_number = 0;
        Flag flags = Flag::None;
        uint16 generation = 0;
    };
    /// \brief entity‚ªíœ‚³‚ê‚Ä‚¢‚½ê‡‚ÍChunkContainer“à‚ÅíœÏ‚ÝƒŠƒXƒg‚Ì—v‘f‚Æ‚µ‚Äˆµ‚¤
    /// \note https://skypjack.github.io/2019-05-06-ecs-baf-part-3/
    struct DeletedElement {
        EntityIndex next_deleted_index ;
        uint8 world_number = 0;
        Flag flags = Flag::None;
    };
    union Impl {
        IndexGeneration index_generation;
        DeletedElement deleted_element;
    };

    Impl value = {};
};

namespace std {
template<>
struct hash<Entity> {
    std::size_t operator()(const Entity& entity) const
    {
        auto h0 = std::hash<uint16>()(entity.value.index_generation.entity_index.index);
        auto h1 = std::hash<uint16>()(entity.value.index_generation.entity_index.chunk_index);
        auto h2 = std::hash<uint32>()(entity.value.index_generation.entity_index.archetype_number);
        auto h3 = std::hash<uint8>()(entity.value.index_generation.world_number);
        auto h4 = std::hash<uint16>()(entity.value.index_generation.generation);
        return h0 ^ (h1 << 1) ^ (h2 << 2) ^ (h3 << 3) ^ (h4 << 4);
    }
};
}