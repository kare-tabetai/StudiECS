#pragma once
#include "Type.h"
#include "Constant.h"
#include "EnumUtil.h"
#include <cassert>

class Entity {
public:
    enum class Flag : uint8 {
        None = 0,
        Valid = 1 << 0,// ON‚È‚ç—LŒø
    };

    struct Impl {
        Impl() = default;
        constexpr Impl(uint32 _record_index, uint16 _generation, Flag _flag, WorldNumber _world_number)
            : record_index(_record_index)
            , generation(_generation)
            , flag(_flag)
            , world_number(_world_number)
        {
        }
        RecordIndex record_index;
        Generation generation;
        Flag flag;
        WorldNumber world_number;
    };

    Entity() = default;

    constexpr Entity(WorldNumber world_number, Flag flag)
        : impl(0,0,flag,world_number)
    {
    }

    explicit operator uint64() const
    {
        return id;
    }

    void ReSet(RecordIndex record_index,Generation generation, WorldNumber world_number)
    {
        impl = Impl(record_index, generation, Flag::Valid, world_number);
    }

    void Release()
    {
        RemoveFlag(Flag::Valid);
    }

    /// \brief –³Œø‚Èentity‚©‚Ç‚¤‚©
    bool IsInvalid() const
    {
        return IsFlagsOff(impl.flag , Flag::Valid);
    }

    Entity& SetFlag(Flag flag)
    {
        impl.flag = flag;
        return *this;
    }
    Entity& AddFlag(Flag flag)
    {
        impl.flag |= flag;
        return *this;
    }
    Entity& RemoveFlag(Flag flag)
    {
        impl.flag &= ~flag;
        return *this;
    }

    RecordIndex GetRecordIndex() const
    {
        return impl.record_index;
    }

    Generation GetGeneration()const
    {
        return impl.generation;
    }

    auto operator<=>(const Entity&) const = default;

    union {
        Impl impl;
        uint64 id;
    };
};

// chunk“à‚Éentity©‘Ì‚ğ–„‚ß‚İmemcpy“™‚ğ‚µ‚½‚¢‚½‚ßtrivialŒn‚Ì§ŒÀ‚ğ‚µ‚Ä‚¢‚é
static_assert(std::is_trivially_move_constructible_v<Entity>);
static_assert(std::is_trivially_destructible_v<Entity>);
static_assert(std::is_trivially_copyable_v<Entity>);

namespace std {
template<>
struct hash<Entity> {
    std::size_t operator()(const Entity& entity) const
    {
        return std::hash<uint64>()(static_cast<uint64>(entity));
    }
};
}