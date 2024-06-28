#pragma once
#include "EnumUtil.h"
#include "EntityIndex.h"
#include "Type.h"
#include <bitset>
#include <boost/hana/hash.hpp>
#include <cassert>

/// \note entity‚ªíœ‚³‚ê‚Ä‚¢‚½ê‡‚ÍChunkContainer“à‚ÅíœÏ‚İƒŠƒXƒg‚Ì—v‘f‚Æ‚µ‚Äˆµ‚¤
/// https://skypjack.github.io/2019-05-06-ecs-baf-part-3/
class Entity {
public:
    friend struct std::hash<Entity>;

    enum class Flag : uint8 {
        None = 0,
        Invalid = 1 << 0,
        IsInChunk = 1 << 1,
    };

    Entity() = default;
    
    void Release() {
        assert(IsInvalid());
    }

    static constexpr Entity Invalid()
    {
        Entity entity;
        entity.m_flags = Flag::Invalid;
        return entity;
    }

    EntityIndex Index() const { return m_entity_index; }
    EntityIndex SwapIndex(EntityIndex entity_index)
    { 
        auto tmp = m_entity_index;
        m_entity_index = entity_index;
        return tmp; 
    }

    /// \brief –³Œø‚Èentity‚©‚Ç‚¤‚©
    bool IsInvalid() const
    {
        return ToBool(m_flags | Flag::Invalid);
    }

    Entity& SetFlag(Flag flag)
    {
        m_flags = flag;
        return *this;
    }
    Entity& AddFlag(Flag flag)
    {
        m_flags |= flag;
        return *this;
    }
    Entity& RemoveFlag(Flag flag)
    {
        m_flags &= ~flag;
        return *this;
    }

    bool operator==(const Entity& other) const
    {
        return m_entity_index == other.m_entity_index 
            && m_world_number == other.m_world_number
            && m_generation == other.m_generation;
    }

private:
    EntityIndex m_entity_index;
    WorldNumber m_world_number = 0;
    Flag m_flags = Flag::None;
    uint16 m_generation = 0;
};

namespace std {
template<>
struct hash<Entity> {
    std::size_t operator()(const Entity& entity) const
    {
        auto h0 = std::hash<uint16>()(entity.m_entity_index.index);
        auto h1 = std::hash<uint16>()(entity.m_entity_index.chunk_index);
        auto h2 = std::hash<uint32>()(entity.m_entity_index.archetype_number);
        auto h3 = std::hash<uint8>()(entity.m_world_number);
        auto h4 = std::hash<uint16>()(entity.m_generation);
        return h0 ^ (h1 << 1) ^ (h2 << 2) ^ (h3 << 3) ^ (h4 << 4);
    }
};
}